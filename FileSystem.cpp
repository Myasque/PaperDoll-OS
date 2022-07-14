#include "FileSystem.h"

//�ļ�����ȫ�ֱ���
FCB* POS_POINTER = NULL;

extern memoryManager myMemoryManager;

DISK::DISK()
{
	//��ʼ�����пռ��
	ifstream ifs(fslLoc);
	for (int i = 0; i < BLOCK_SEQ; i++)
	{
		string iStr;
		getline(ifs, iStr);
		int ifsEntry;
		istringstream iIStr(iStr);
		iIStr >> ifsEntry;
		this->freeSpaceList.push_back(ifsEntry);
	}
	//��ʼ������
	for (int i = 0; i < TRACK_NUM; i++)
	{
		vector<BLOCK> temp;
		for (int j = 0; j < BLOCK_NUM; j++)
		{
			BLOCK Block(i * BLOCK_NUM + j);
			temp.push_back(Block);
		}
		this->virtualDisk.push_back(temp);
	}
}

DISK::~DISK()
{
	//д����пռ��
	ofstream ofs(fslLoc, ios::trunc);
	auto iter_fsl = this->freeSpaceList.begin();
	for (; iter_fsl != this->freeSpaceList.end(); ++iter_fsl)
	{
		ofs << *iter_fsl << endl;
	}
	ofs.close();

	//д�����
	//�о�Ӧ�ò������ֶ�д������
}

BLOCK* DISK::findBlock(int blockNum)
{
	int trackNo = blockNum / BLOCK_NUM; //�ŵ��ż���ų���ÿ����������0~31����0�ţ�32~63��1��
	int blockNo = blockNum % BLOCK_NUM;
	BLOCK* result = &(this->virtualDisk[trackNo][blockNo]);
	return result;
}

int DISK::allocBlock(string context)
{
	int result = DISK_NO_SPACE;
	BLOCK* aBlock;
	for (int i = 0; i < this->freeSpaceList.size(); i++)
	{
		if (this->freeSpaceList[i] == true) //��λֵΪTRUE������
		{
			result = i;
			this->freeSpaceList[i] = false; //��ռ��
			break;
		}
	}
	if (result != DISK_NO_SPACE) //˵�����пռ�
	{
		aBlock = this->findBlock(result);
		aBlock->blockCon = context;
	}
	return result;
}

string DISK::readBlock(int blockNum, int mode)
{
	string result;
	stringstream temp;
	BLOCK* rBlock = this->findBlock(blockNum);
	int len = 0;
	if (mode == READ_E) //����ִ���ļ�
	{
		for (int i = 0; i < rBlock->blockCon.size(); i++)
		{
			if (rBlock->blockCon[i] == '\n')
			{
				while (len < MAX_DIR_LEN - 1) //����31λ
				{
					temp << ' ';
					len++;
				}
				temp << '\n';
				len = 0;
			}
			else
			{
				temp << rBlock->blockCon[i];
				len++;
			}
		}
		while (len < MAX_DIR_LEN - 1) //���һ��ָ��ҲҪ����
		{
			temp << ' ';
			len++;
		}
		temp << '\n';
		result = temp.str();
	}
	else
	{
		result = rBlock->blockCon;
	}
	return result;
}

void DISK::eraseBlock(int blockNum)
{
	BLOCK* eBlock = this->findBlock(blockNum);
	eBlock->blockCon = ""; //ɾ���������ݣ�û�б�Ҫ����Ϊ�˰�ȫ�ԣ���Ȼ���fs��������ȫ��
	this->freeSpaceList[blockNum] = true; //�ͷŴ��̿�
}




FCB::FCB(string fileName, int fileSize, char fileAuth, vector<int> fileIndex)
{
	this->name = fileName;
	int nameLen = fileName.size();
	this->type = fileName[nameLen - 1];
	this->parent = POS_POINTER;
	this->size = fileSize;
	if (!this->parent) this->auth = AUTH_ALL;
	else if (fileAuth != -1) this->auth = fileAuth;
	else this->auth = POS_POINTER->auth;
	if (fileIndex[0] != -1)//��ʼ��ʱ������FCB�Ѿ���������
		this->index = fileIndex;
}

string FCB::printRoute()
{
	string route = this->name;
	string tmpRoute = "";
	FCB* printIter = this->parent; //ָ��Ŀ¼
	while (printIter != NULL) //�𼶷��ʣ�ֱ�������Ŀ¼
	{
		tmpRoute = printIter->name; //���游Ŀ¼������
		route = tmpRoute + "\\" + route; //·���м��븸Ŀ¼
		printIter = printIter->parent; //ָ������һ��Ŀ¼
	}
	return route;
}

string FCB::printDirectory(string dirRoute, int mode)
{
	auto iter = this->directoryTree.begin();
	string result = "";
	for (; iter != this->directoryTree.end(); ++iter) //������ǰĿ¼
	{
		string route = dirRoute + "/";
		if (iter->second.type != 'f') //��������ģʽ����ͨ�ļ��Ĵ���һ��
			//��result�������Ŀ¼���ָ������ļ��������з�
			result = result + route + iter->first + "$";
		else //Ŀ¼�ļ��Ĵ�����ģʽ���
		{
			result = result + route + iter->first + "$";
			if (mode == CHIL_DIR)//�ݹ���ã���ӡ��ǰĿ¼����Ŀ¼������Ŀ
			{
				route = dirRoute + "/" + iter->first;
				result = result + iter->second.printDirectory(route, mode);
			}
		}
	}
	//result.insert(-1, "~$");
	return result;
}

vector<string> FCB::printFile(const char* fileType)
{
	int len = strlen(fileType);
	auto iter = this->directoryTree.begin();
	vector<string> result;
	for (; iter != this->directoryTree.end(); ++iter)//������ǰĿ¼
		for (int i = 0; i < len; i++)//��ÿ�����ͶԱ�
			if (iter->second.type == fileType[i])
				result.push_back(iter->first);
	return result;
}

vector<FCB> FCB::saveDirectory()
{
	auto iter = this->directoryTree.begin();
	vector<FCB> result;
	vector<FCB> sonRet;
	for (; iter != this->directoryTree.end(); ++iter) //������ǰĿ¼
	{
		result.push_back(iter->second);
		if (iter->second.type == 'f') //Ŀ¼����Ҫ��������
		{
			sonRet = iter->second.saveDirectory(); //�ݹ����
			result.insert(result.end(), sonRet.begin(), sonRet.end()); //����Ŀ¼��׺�ں���
		}
	}
	return result;
}

FCB* FCB::findFile(string fileName)
{
	auto iter = this->directoryTree.begin();
	FCB* result = NULL;
	for (; iter != this->directoryTree.end(); ++iter) //������ǰĿ¼
		if (fileName == iter->first)
			result = &(iter->second); //����ҵ���Ŀ���ļ����򷵻�fcb
	return result;
}

int FCB::deleteFCB(string fileName)
{
	auto iter = this->directoryTree.begin();
	for (; iter != this->directoryTree.end(); ++iter) //������ǰĿ¼
		if (fileName == iter->first)
		{
			this->directoryTree.erase(iter);
			break;
		}
	return SUC;
}



OftEntry* OFT::searchOft(string fileName)
{
	vector<OftEntry>::iterator iter = this->OfTable.begin();
	OftEntry* result = NULL;
	int i = 0;
	for (; iter != this->OfTable.end(); ++iter, ++i) //����OFT
	{
		if (iter->file->name == fileName) //�ҵ���
			result = &(this->OfTable[i]);
	}
	return result;
}

int OFT::deleteOft(string fileName)
{
	auto iter = this->OfTable.begin();
	for (; iter != this->OfTable.end(); ++iter) //������ǰĿ¼
		if (fileName == iter->file->name)
		{
			this->OfTable.erase(iter);
			break;
		}
	return SUC;
}



FileSystem::FileSystem()
{
	POS_POINTER = &(this->myDir); //��ʼ��POS_POINTER

	ifstream ifs(dirLoc);
	string name;
	string temp = "~";

	while (getline(ifs, name))
	{
		string parent;
		string authStr;
		string sizeStr;
		string indexStr;
		int size = 0;
		int auth = 0;
		vector<int> index;

		getline(ifs, parent);
		getline(ifs, authStr);
		istringstream authIStr(authStr);
		authIStr >> auth;
		getline(ifs, sizeStr);
		istringstream sizeIStr(sizeStr);
		sizeIStr >> size;
		getline(ifs, indexStr);
		int len = indexStr.size();
		for (int i = 0; i < len; i++)
		{
			string tempStr;
			while (indexStr[i] != ' ' && i < len)
			{
				tempStr += indexStr.substr(i, 1);
				i++;
			}
			istringstream indexIStr(tempStr);
			int tempInt;
			indexIStr >> tempInt;
			index.push_back(tempInt);
		}
		if (index.empty()) index.push_back(-1);

		while (parent != POS_POINTER->name) //˵�����ڴ�Ŀ¼��
		{
			if (parent == temp) //����Ŀ¼
			{
				auto iter = POS_POINTER->directoryTree.find(temp);
				POS_POINTER = &(iter->second);
			}
			else //�Ǹ�Ŀ¼�����ܶ༶
			{
				POS_POINTER = POS_POINTER->parent;
			}
		}
		/*
		* ��ǵ�������
		*/
		FCB tempFcb(name, size, auth, index);
		POS_POINTER->directoryTree.insert({ name,tempFcb });
		getline(ifs, temp);
		temp = name;
	}
	ifs.close();
	while (POS_POINTER->parent != NULL)
	{
		POS_POINTER = POS_POINTER->parent;
	}
}

FileSystem::~FileSystem()
{
	ofstream ofs(dirLoc, ios::trunc); //��ԭ�����ļ�ɾ�ˣ�����д�뱾�ε�
	vector<FCB> save = this->myDir.saveDirectory(); //�õ��ļ�ϵͳ�Ĵ洢�ṹ
	auto iter = save.begin();
	string result;
	for (; iter != save.end(); ++iter)
	{
		result += iter->name + "\n" + iter->parent->name + "\n";
		result += to_string(iter->auth) + "\n";
		result += to_string(iter->size) + "\n";

		int len = iter->index.size();
		for (int i = 0; i < len; i++)
		{
			result += to_string(iter->index[i]) + " ";
		}
		result += "\n\n";
	}
	ofs << result;
	ofs.close();
}

int FileSystem::creatFile(string fileName)
{
	int nameLen = fileName.size();
	if (fileName[0] == '.' || fileName[0] == '\\' || fileName.size() > MAX_NAME_LEN || fileName[nameLen - 2] != '.'
		|| (fileName[nameLen - 1] != 'e' && fileName[nameLen - 1] != 't' && fileName[nameLen - 1] != 'f'))
	{
		qDebug() << QString::fromStdString(fileName) << "ERROR Create File :illegal name" << endl;
		return C_ERR_ILL_NAME; //�ļ����Ƿ����޶������ַ������ȣ��������ļ�����
	}
	FCB* dupFile = POS_POINTER->findFile(fileName);
	if (dupFile != NULL)
	{
		qDebug() << QString::fromStdString(fileName) << "ERROR Create File :illegal name" << endl;
		return C_ERR_DUP_NAME; //�ļ����ظ�
	}
	FCB newFcb(fileName); //����fcb
	POS_POINTER->directoryTree.insert({ fileName, newFcb });
	qDebug() << QString::fromStdString(fileName) << "New File success��" << endl;
	return SUC;
}

void FileSystem::releaseDisk(FCB* dFile)
{
	/*
	* ����һ��ɾĿ¼
	*/
	if (dFile->type == 'f') //Ŀ¼�ļ�����ɾ���ļ�
	{
		auto iter = dFile->directoryTree.begin();
		for (; iter != dFile->directoryTree.end(); ++iter)
		{
			releaseDisk(&(iter->second));
		}
	}

	//�ͷű�������
	auto iter = dFile->index.begin();
	for (; iter != dFile->index.end(); ++iter)
	{
		myDisk.eraseBlock(*iter);
	}
	dFile->index.clear();
}

int FileSystem::deleteFile(string fileName)
{
	FCB* dFile = POS_POINTER->findFile(fileName);
	if (!dFile)
	{
		//cout << fileName << "ɾ���ļ�ʧ�ܣ��ļ������ڣ�" << endl;
		return D_ERR_NO_FILE; //����ļ������ڣ�ʵ����Ӧ�ò��ᷢ����
	}
	if (dFile->auth == AUTH_NO_DEL)
	{
		//cout << fileName << "ɾ���ļ�ʧ�ܣ���û��Ȩ��!" << endl;
		return D_ERR_LACK_AUTH; //����ɾ���ļ�
	}
	OftEntry* oFile = this->myOft.searchOft(fileName);
	if (oFile != NULL) //��ʵ�����ڴ��ļ��������ڱ�ռ�ã����Ǳ��������һ������flag
	{
		if (oFile->getReadFlag() || oFile->getWriteFlag()) //��һ����Ϊ��
		{
			//cout << fileName << "ɾ���ļ�ʧ�ܣ��ļ����ڱ�ռ�ã�" << endl;
			return D_ERR_IS_OCCUPY;
		}
	}

	this->releaseDisk(dFile);

	POS_POINTER->deleteFCB(fileName);

	//cout << fileName << "ɾ���ļ��ɹ���" << endl;
	return SUC;
}

string FileSystem::readDisk(FCB* rFile)
{
	string result;
	auto iter = rFile->index.begin();
	for (; iter != rFile->index.end(); ++iter)
	{
		if (rFile->type == 'e')
		{
			result += this->myDisk.readBlock(*iter, READ_E);
		}
		else
		{
			result += this->myDisk.readBlock(*iter);
		}
	}
	return result;
}

FL FileSystem::readFile(string fileName, int pid)
{
	FCB* rFile;
	if (fileName[0] == '.')//����ǡ�.����ͷ��ϵͳ�ļ��Ļ�
	{
		//��ָ���ļ�����
		rFile = this->myDir.findFile(".dir.f")->findFile(fileName);
	}
	else { rFile = POS_POINTER->findFile(fileName); }

	FL result;
	result.start = R_ERR_IS_OCCUPY;
	result.end = R_ERR_IS_OCCUPY;

	//�ڴ��ļ����в����ļ�״̬
	OftEntry* oFile = this->myOft.searchOft(fileName);
	if (oFile != NULL) //�ҵ��˵Ļ���Ҫ��һ���Ƿ�����д��
	{
		if (oFile->getWriteFlag())
		{
			//cout << fileName << "��ȡ�ļ�ʧ�ܣ��ļ����ڱ�д�룡" << endl;
			return result; //����д�룬������
		}
	}
	else //����OFT��½�����
	{
		OftEntry newEntry(rFile);
		this->myOft.OfTable.push_back(newEntry);
		oFile = this->myOft.searchOft(fileName);
	}
	oFile->R_occuppy(); //�޸Ķ���ʶ��

	string context;
	context = this->readDisk(oFile->file);
	IRnum = context.size() / INS_LEN;
	qDebug() << "FileSystem knows the IRnum is:" << IRnum;
	qDebug() << "FileSystem knows the context is:" << QString::fromStdString(context);
	qDebug() << "FileSystem passes it to writeVirtual";
	result = myMemoryManager.writeVirtual(pid, context);
	result.filename = fileName;

	//Sleep(5000); //���Զ��߳��õģ��ǵ�ɾ
	oFile->R_release();
	if (!oFile->getReadFlag())//�ͷź��ź���Ϊ0
	{
		this->myOft.deleteOft(fileName);//��ʽ�عر��ļ�
	}

	/*
	* ������ɺ���ɾ��
	*/
	//cout << fileName << "��ȡ�ļ��ɹ���" << context << endl;
	qDebug() << "FileSystm knows that memory put " << QString::fromStdString(context) << "in ";
	qDebug() << QString::number(result.start) << " , " << QString::number(result.end);
	return result;
}

int FileSystem::writeFile(string fileName, string writeObj, int mode, int pid)
{
	FCB* wFile = POS_POINTER->findFile(fileName);

	/*
	* ע�⣺��Ҫд��ֻ���ļ�ʱ����ע���������У�����
	*/

	if (wFile->auth == AUTH_READ_ONLY)
	{
		//cout << fileName << "д���ļ�������û��д��Ȩ�ޣ�" << endl;
		return W_ERR_LACK_AUTH;
	}



	//�ڴ��ļ����в����ļ�״̬
	OftEntry* oFile = this->myOft.searchOft(fileName);
	if (oFile != NULL) //�ڱ�ռ�ã����Ǳ��������һ������flag
	{
		if (oFile->getReadFlag() || oFile->getWriteFlag()) //��һ����Ϊ��
		{
			//cout << fileName << "д���ļ�ʧ�ܣ��ļ����ڱ�ռ�ã�" << endl;
			return D_ERR_IS_OCCUPY;
		}
	}
	else //����OFT��½�����
	{
		OftEntry newEntry(wFile);
		this->myOft.OfTable.push_back(newEntry);
		oFile = this->myOft.searchOft(fileName);
	}
	oFile->W_occuppy(); //�޸Ķ���ʶ��

	string context;
	if (mode == WRITE_TAIL) context = this->readDisk(wFile);
	context += writeObj;

	this->releaseDisk(wFile);
	int len = context.size();
	int blockSize = BLOCK_SIZE;
	int blockNum = (len / blockSize) + 1;

	for (int i = 0; i < blockNum; i++)
	{
		string temp = context.substr(blockSize * i, blockSize);
		int newBlock = this->myDisk.allocBlock(temp);
		if (newBlock == DISK_NO_SPACE)
		{
			//cout << fileName << "д���ļ�ʧ�ܣ��洢�ռ䲻�㣡" << endl;
			oFile->W_release();
			return W_ERR_NO_SPACE;
		}
		wFile->index.push_back(newBlock);
	}

	wFile->size = context.size();

	//Sleep(5000); //���Զ��߳��õģ��ǵ�ɾ
	oFile->W_release();
	if (!oFile->getWriteFlag())//�ͷź��ź���Ϊ0
	{
		this->myOft.deleteOft(fileName);//��ʽ�عر��ļ�
	}

	return SUC;
}

int FileSystem::changeRoute(string route)
{
	if (route == "..") //�����ϼ�Ŀ¼
	{
		POS_POINTER = POS_POINTER->parent;
		return SUC;
	}
	FCB* newRoute = POS_POINTER->findFile(route); //�ڱ�Ŀ¼�����ļ�
	if (newRoute != NULL)
	{
		POS_POINTER = newRoute;
		return SUC;
	}
	return ERR;
}
