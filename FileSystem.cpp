#include "FileSystem.h"

//文件部分全局变量
FCB* POS_POINTER = NULL;

extern memoryManager myMemoryManager;

DISK::DISK()
{
	//初始化空闲空间表
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
	//初始化磁盘
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
	//写入空闲空间表
	ofstream ofs(fslLoc, ios::trunc);
	auto iter_fsl = this->freeSpaceList.begin();
	for (; iter_fsl != this->freeSpaceList.end(); ++iter_fsl)
	{
		ofs << *iter_fsl << endl;
	}
	ofs.close();

	//写入磁盘
	//感觉应该不用我手动写……？
}

BLOCK* DISK::findBlock(int blockNum)
{
	int trackNo = blockNum / BLOCK_NUM; //磁道号即块号除以每道块数，如0~31块在0号，32~63在1号
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
		if (this->freeSpaceList[i] == true) //该位值为TRUE，空闲
		{
			result = i;
			this->freeSpaceList[i] = false; //被占用
			break;
		}
	}
	if (result != DISK_NO_SPACE) //说明还有空间
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
	if (mode == READ_E) //读可执行文件
	{
		for (int i = 0; i < rBlock->blockCon.size(); i++)
		{
			if (rBlock->blockCon[i] == '\n')
			{
				while (len < MAX_DIR_LEN - 1) //补足31位
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
		while (len < MAX_DIR_LEN - 1) //最后一条指令也要处理
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
	eBlock->blockCon = ""; //删除磁盘内容（没有必要，是为了安全性，虽然这个fs根本不安全）
	this->freeSpaceList[blockNum] = true; //释放磁盘块
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
	if (fileIndex[0] != -1)//初始化时建立的FCB已经有了索引
		this->index = fileIndex;
}

string FCB::printRoute()
{
	string route = this->name;
	string tmpRoute = "";
	FCB* printIter = this->parent; //指向父目录
	while (printIter != NULL) //逐级访问，直至到达根目录
	{
		tmpRoute = printIter->name; //保存父目录的名称
		route = tmpRoute + "\\" + route; //路径中加入父目录
		printIter = printIter->parent; //指向再上一级目录
	}
	return route;
}

string FCB::printDirectory(string dirRoute, int mode)
{
	auto iter = this->directoryTree.begin();
	string result = "";
	for (; iter != this->directoryTree.end(); ++iter) //遍历当前目录
	{
		string route = dirRoute + "/";
		if (iter->second.type != 'f') //不管哪种模式，普通文件的处理都一样
			//在result后面加上目录、分隔符、文件名、换行符
			result = result + route + iter->first + "$";
		else //目录文件的处理与模式相关
		{
			result = result + route + iter->first + "$";
			if (mode == CHIL_DIR)//递归调用，打印当前目录及子目录所有条目
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
	for (; iter != this->directoryTree.end(); ++iter)//遍历当前目录
		for (int i = 0; i < len; i++)//与每种类型对比
			if (iter->second.type == fileType[i])
				result.push_back(iter->first);
	return result;
}

vector<FCB> FCB::saveDirectory()
{
	auto iter = this->directoryTree.begin();
	vector<FCB> result;
	vector<FCB> sonRet;
	for (; iter != this->directoryTree.end(); ++iter) //遍历当前目录
	{
		result.push_back(iter->second);
		if (iter->second.type == 'f') //目录则需要访问内容
		{
			sonRet = iter->second.saveDirectory(); //递归调用
			result.insert(result.end(), sonRet.begin(), sonRet.end()); //将子目录连缀在后面
		}
	}
	return result;
}

FCB* FCB::findFile(string fileName)
{
	auto iter = this->directoryTree.begin();
	FCB* result = NULL;
	for (; iter != this->directoryTree.end(); ++iter) //遍历当前目录
		if (fileName == iter->first)
			result = &(iter->second); //如果找到了目标文件，则返回fcb
	return result;
}

int FCB::deleteFCB(string fileName)
{
	auto iter = this->directoryTree.begin();
	for (; iter != this->directoryTree.end(); ++iter) //遍历当前目录
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
	for (; iter != this->OfTable.end(); ++iter, ++i) //遍历OFT
	{
		if (iter->file->name == fileName) //找到了
			result = &(this->OfTable[i]);
	}
	return result;
}

int OFT::deleteOft(string fileName)
{
	auto iter = this->OfTable.begin();
	for (; iter != this->OfTable.end(); ++iter) //遍历当前目录
		if (fileName == iter->file->name)
		{
			this->OfTable.erase(iter);
			break;
		}
	return SUC;
}



FileSystem::FileSystem()
{
	POS_POINTER = &(this->myDir); //初始化POS_POINTER

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

		while (parent != POS_POINTER->name) //说明不在此目录下
		{
			if (parent == temp) //是子目录
			{
				auto iter = POS_POINTER->directoryTree.find(temp);
				POS_POINTER = &(iter->second);
			}
			else //是父目录，可能多级
			{
				POS_POINTER = POS_POINTER->parent;
			}
		}
		/*
		* 请记得索引！
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
	ofstream ofs(dirLoc, ios::trunc); //把原本的文件删了，重新写入本次的
	vector<FCB> save = this->myDir.saveDirectory(); //得到文件系统的存储结构
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
		return C_ERR_ILL_NAME; //文件名非法，限定了首字符、长度，必须有文件类型
	}
	FCB* dupFile = POS_POINTER->findFile(fileName);
	if (dupFile != NULL)
	{
		qDebug() << QString::fromStdString(fileName) << "ERROR Create File :illegal name" << endl;
		return C_ERR_DUP_NAME; //文件名重复
	}
	FCB newFcb(fileName); //创建fcb
	POS_POINTER->directoryTree.insert({ fileName, newFcb });
	qDebug() << QString::fromStdString(fileName) << "New File success！" << endl;
	return SUC;
}

void FileSystem::releaseDisk(FCB* dFile)
{
	/*
	* 测试一下删目录
	*/
	if (dFile->type == 'f') //目录文件，先删子文件
	{
		auto iter = dFile->directoryTree.begin();
		for (; iter != dFile->directoryTree.end(); ++iter)
		{
			releaseDisk(&(iter->second));
		}
	}

	//释放本体内容
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
		//cout << fileName << "删除文件失败：文件不存在！" << endl;
		return D_ERR_NO_FILE; //如果文件不存在（实际上应该不会发生）
	}
	if (dFile->auth == AUTH_NO_DEL)
	{
		//cout << fileName << "删除文件失败：您没有权限!" << endl;
		return D_ERR_LACK_AUTH; //不可删除文件
	}
	OftEntry* oFile = this->myOft.searchOft(fileName);
	if (oFile != NULL) //其实就是在打开文件表，就是在被占用，但是保险起见看一眼两个flag
	{
		if (oFile->getReadFlag() || oFile->getWriteFlag()) //有一个不为零
		{
			//cout << fileName << "删除文件失败：文件正在被占用！" << endl;
			return D_ERR_IS_OCCUPY;
		}
	}

	this->releaseDisk(dFile);

	POS_POINTER->deleteFCB(fileName);

	//cout << fileName << "删除文件成功！" << endl;
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
	if (fileName[0] == '.')//如果是‘.’开头的系统文件的话
	{
		//在指令文件夹找
		rFile = this->myDir.findFile(".dir.f")->findFile(fileName);
	}
	else { rFile = POS_POINTER->findFile(fileName); }

	FL result;
	result.start = R_ERR_IS_OCCUPY;
	result.end = R_ERR_IS_OCCUPY;

	//在打开文件表中查找文件状态
	OftEntry* oFile = this->myOft.searchOft(fileName);
	if (oFile != NULL) //找到了的话，要看一眼是否正被写入
	{
		if (oFile->getWriteFlag())
		{
			//cout << fileName << "读取文件失败：文件正在被写入！" << endl;
			return result; //正被写入，报错返回
		}
	}
	else //不在OFT里，新建表项
	{
		OftEntry newEntry(rFile);
		this->myOft.OfTable.push_back(newEntry);
		oFile = this->myOft.searchOft(fileName);
	}
	oFile->R_occuppy(); //修改读标识符

	string context;
	context = this->readDisk(oFile->file);
	IRnum = context.size() / INS_LEN;
	qDebug() << "FileSystem knows the IRnum is:" << IRnum;
	qDebug() << "FileSystem knows the context is:" << QString::fromStdString(context);
	qDebug() << "FileSystem passes it to writeVirtual";
	result = myMemoryManager.writeVirtual(pid, context);
	result.filename = fileName;

	//Sleep(5000); //测试多线程用的，记得删
	oFile->R_release();
	if (!oFile->getReadFlag())//释放后，信号量为0
	{
		this->myOft.deleteOft(fileName);//隐式地关闭文件
	}

	/*
	* 测试完成后请删除
	*/
	//cout << fileName << "读取文件成功：" << context << endl;
	qDebug() << "FileSystm knows that memory put " << QString::fromStdString(context) << "in ";
	qDebug() << QString::number(result.start) << " , " << QString::number(result.end);
	return result;
}

int FileSystem::writeFile(string fileName, string writeObj, int mode, int pid)
{
	FCB* wFile = POS_POINTER->findFile(fileName);

	/*
	* 注意：需要写入只读文件时，请注释下面这行！！！
	*/

	if (wFile->auth == AUTH_READ_ONLY)
	{
		//cout << fileName << "写入文件错误：您没有写入权限！" << endl;
		return W_ERR_LACK_AUTH;
	}



	//在打开文件表中查找文件状态
	OftEntry* oFile = this->myOft.searchOft(fileName);
	if (oFile != NULL) //在被占用，但是保险起见看一眼两个flag
	{
		if (oFile->getReadFlag() || oFile->getWriteFlag()) //有一个不为零
		{
			//cout << fileName << "写入文件失败：文件正在被占用！" << endl;
			return D_ERR_IS_OCCUPY;
		}
	}
	else //不在OFT里，新建表项
	{
		OftEntry newEntry(wFile);
		this->myOft.OfTable.push_back(newEntry);
		oFile = this->myOft.searchOft(fileName);
	}
	oFile->W_occuppy(); //修改读标识符

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
			//cout << fileName << "写入文件失败：存储空间不足！" << endl;
			oFile->W_release();
			return W_ERR_NO_SPACE;
		}
		wFile->index.push_back(newBlock);
	}

	wFile->size = context.size();

	//Sleep(5000); //测试多线程用的，记得删
	oFile->W_release();
	if (!oFile->getWriteFlag())//释放后，信号量为0
	{
		this->myOft.deleteOft(fileName);//隐式地关闭文件
	}

	return SUC;
}

int FileSystem::changeRoute(string route)
{
	if (route == "..") //返回上级目录
	{
		POS_POINTER = POS_POINTER->parent;
		return SUC;
	}
	FCB* newRoute = POS_POINTER->findFile(route); //在本目录查找文件
	if (newRoute != NULL)
	{
		POS_POINTER = newRoute;
		return SUC;
	}
	return ERR;
}
