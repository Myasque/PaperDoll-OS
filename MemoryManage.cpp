# include "MemoryManage.h"

//�ڴ沿��ȫ�ֱ���
const char mode = 'p'; //��ҳmode=p ����mode=c
const int pageSize = 1024;
int replaceStrategy = 1;
//0->FIFO 1->LRU
long allocated = 0;
int virtualMemory[VIRTUAL_NUMBER][3];
int physicalMemory[PHYSICAL_NUMBER];
list<int> schedule_queue;
int physicalSize = 0;
int page_fault = 0;
string Virtual[VIRTUAL_NUMBER];
string Physical[PHYSICAL_NUMBER];

using namespace std;

int Tools::find_physical(int elem)
{

	for (int i = 0; i < PHYSICAL_NUMBER; i++)
	{
		if (physicalMemory[i] == elem)
		{
			return i;
		}
	}
	return -1;
}


//allocated virtual page number
void pageTable::insert_virtual(int pageNumber)
{
	table.insert(pair<int, int>(pageNumber, -1));
}

void pageTable::delete_page(int pageNumber)
{
	table[pageNumber] = -1;
}

int pageTable::transform(int pid, int address, int pageSize)
{
	int idx = address / pageSize; //�ڽ��̵ĵڼ�ҳ���������λ�ã�
	int nSize = table.size();
	//printf("table size=%d\n", nSize);
	if (idx < nSize)
	{
		int tmptable[VIRTUAL_NUMBER], i = 0;

		std::map < int, int > ::iterator it;
		std::map < int, int > ::iterator itEnd;
		it = table.begin();
		itEnd = table.end();
		while (it != itEnd) {
			tmptable[i] = it->first;
			i++;
			it++;
		}
		//printf("return virtual_page_num=%d\n", tmptable[idx]);
		return tmptable[idx]; //��������ҳ��
	}
	else
		return -1;
}


void pageTable::modify(int vNum, int pNum, int valid)
{
	if (valid == 1)
	{
		table[vNum] = pNum; //page in
	}
	else
	{
		table[vNum] = -1; //page out
	}
}

int pageTable::find_physical(int index)
{
	return table[index];
}


void memoryManager::Initial()
{
	int i, j;
	//paging storage
	if (mode == 'p')
	{
		for (i = 0; i < VIRTUAL_NUMBER; i++) {
			for (j = 0; j < 3; j++) {
				if (j == 0) virtualMemory[i][j] = 0;
				if (j == 1) virtualMemory[i][j] = -1;
				if (j == 2) virtualMemory[i][j] = 0;
			}
			Virtual[i] = "*";
		}

		for (i = 0; i < PHYSICAL_NUMBER; i++)
		{
			physicalMemory[i] = -1;
			Physical[i] = "*";
		}

	}

	//error storage type
	else
	{
		printf("mode error");
	}
}

int memoryManager::pageAlloc(int pid, int size)
{
	int i, j;
	int s = size;
	pageTable ptable;
	for (i = 0; i < VIRTUAL_NUMBER; i++)
	{
		if (virtualMemory[i][1] == -1)
		{
			virtualMemory[i][1] = pid;
			if (allPageTable.count(pid) > 0) //�ý����Ѿ��������ڴ����в���ҳ�ˣ��ټ�һ���µľ���
			{
				ptable = allPageTable[pid];
			}
			else
			{
				allPageTable.insert(pair<int, pageTable>(pid, ptable));
			}
			ptable.insert_virtual(i);
			allPageTable[pid] = ptable;
			if (s >= pageSize)
			{
				virtualMemory[i][0] = pageSize;
				s -= pageSize;
			}
			else
			{
				virtualMemory[i][0] = s;
				s = 0;
			}
		}

		if (s == 0)
		{
			allocated += size;
			printf("allocate success!\n");
			return 0;
		}
	}

	pageFree(pid);
	printf("allocate error.\n");
	return -1;

}

int memoryManager::pageFree(int pid)
{
	int i, j;
	int status = 0;
	for (i = 0; i < VIRTUAL_NUMBER; i++)
	{
		if (virtualMemory[i][1] == pid)
		{
			status = 1;
			for (j = 0; j < PHYSICAL_NUMBER; j++)
			{
				//free it from physical memory
				if (physicalMemory[j] == i)
				{
					physicalMemory[j] = -1;
					allPageTable[pid].delete_page(i);
					//�ͷ�ʵ���������ݵ������ڴ�
					Physical[j] = "*";
					//���Ķ���
					schedule_queue.remove(i);
				}
			}
			//free it from virtual memory
			allocated -= virtualMemory[i][0];
			virtualMemory[i][0] = 0;
			virtualMemory[i][1] = -1;
			virtualMemory[i][2] = 0;
			//�ͷ�ʵ���������ݵ������ڴ�
			Virtual[i] = "*";
		}
	}
	if (status == 0)
	{
		printf("Relase memory failed. Process does not exist in memory.\n");
		return -1;
	}
	allPageTable.erase(pid); //ɱ������ 
	printf("Release memory pid[%d].\n", pid);
	return 0;
}

FL memoryManager::writeVirtual(int pid, string context)
{
	int i = 0;
	FL file_location;
	int len = context.length();
	//zmy:����ǿ��ļ�
	//if (len == 0) { file_location.end = 0; file_location.start = 0; return file_location; }
	pageTable ptable = allPageTable[pid];
	map<int, int> Table = ptable.table; //ֻ���ڱ������������޸� 
	// ������������ҳ��
	list<int> index;

	std::map < int, int > ::iterator it;
	std::map < int, int > ::iterator itEnd;
	it = Table.begin();
	itEnd = Table.end();
	while (it != itEnd) {
		index.push_back(it->first);
		i++;
		it++;
	}

	if (index.empty()) {
		printf("write in memory error. Process [%d] does not exist in the memory.\n", pid);
		file_location.start = -1;
		file_location.end = -1;
		return file_location;
	}

	int flag = 0;
	int begin = 0;
	int end = 0;
	int writeLen;
	while (!index.empty() && len > 0) {
		int tmp = index.front();
		if (flag == 0) {
			begin += virtualMemory[tmp][2];
		}
		end += virtualMemory[tmp][2];
		if (virtualMemory[tmp][2] < virtualMemory[tmp][0]) {
			//ֻҪ�޸ģ��ʹ������ڴ���ɾ��
			if (!schedule_queue.empty()) {
				list<int>::iterator it = find(schedule_queue.begin(), schedule_queue.end(), tmp); // ����schedule-queue���Ƿ���Ҫд���ҳ
				if (it != schedule_queue.end()) // �ҵ���
				{
					physicalMemory[ptable.table[tmp]] = -1;
					Physical[ptable.table[tmp]] = "*";
					ptable.table[tmp] = -1;
					allPageTable[pid] = ptable;
					schedule_queue.remove(ptable.table[tmp]);
				}
			}
			//��һ�η��ֿ���ҳ�Ժ�Ͳ��������begin�� 
			flag = 1;
			cout << "����ҳ��: " << tmp << endl;

			if (Virtual[tmp] == "*")	Virtual[tmp].erase(0, 1);
			//׷���ڵ�ǰ�����˵� ���� 
			writeLen = min(len, virtualMemory[tmp][0] - virtualMemory[tmp][2]);
			cout << "д�볤��: " << writeLen << endl;
			Virtual[tmp].append(context, 0, writeLen);
			virtualMemory[tmp][2] += writeLen;
			qDebug() << "LOOOOK AT the memory,tmp=" << QString::number(tmp) << "and the Virtual[tmp]is";
			qDebug() << QString::fromStdString(Virtual[tmp]);
			//ɾ����д�벿��
			context.erase(0, writeLen);
			cout << "ʣ������: " << context << endl;
			len -= writeLen;
			cout << "ʣ�೤��: " << len << "\n" << endl;
			end += writeLen;
		}
		index.pop_front();
		writeLen = 0;
	}

	file_location.start = begin;
	file_location.end = end - 1;

	if (len > 0) {
		printf("�ռ䲻����û��ȫд��\n");
		return file_location;
	}
	end += len;//��ӵĵط�����ȥ

	qDebug() << "The Memory put" << QString::fromStdString(context) << "in{start,end}:";
	qDebug() << QString::number(file_location.start) << " , " << QString::number(file_location.end);
	return file_location;

}


int memoryManager::checkPhysical(int pid, int start, int end)
{
	int page_offset = start % pageSize;
	int ps = pageSize;
	pageTable ptable = allPageTable[pid];
	//�ҵ�����ҳ�� 
	//cout<<"ptable.table[0]: "<< ptable.table[0]<<endl;
	int virtual_startID = ptable.transform(pid, start, ps);
	int virtual_endID = ptable.transform(pid, end, ps);

	//��ȱҳ�ж�
	if (ptable.table[virtual_startID] != -1 && ptable.table[virtual_startID] != -1)
	{
		return 0;
	}
	//ȱҳ�ж�
	else
	{
		return -1;
	}
}

int memoryManager::pageIO(int pid, int start, int end) {
	int page_offset = start % pageSize;
	int ps = pageSize;
	pageTable ptable = allPageTable[pid];
	//�ҵ�����ҳ�� 
	//cout<<"ptable.table[0]: "<< ptable.table[0]<<endl;
	int virtual_startID = ptable.transform(pid, start, ps);
	int virtual_endID = ptable.transform(pid, end, ps);
	if (replaceStrategy == 0)
	{
		ptable = FIFO(virtual_startID, ptable);
		if (virtual_startID != virtual_endID)
			ptable = FIFO(virtual_endID, ptable);
	}
	else if (replaceStrategy == 1)
	{
		ptable = LRU(virtual_startID, ptable);
		if (virtual_startID != virtual_endID)
			ptable = LRU(virtual_endID, ptable);
	}
	allPageTable[pid] = ptable;
	return 0;
}

string memoryManager::pageAccess(int pid, int start, int end)
{
	int len;
	int page_offset = start % pageSize;
	int ps = pageSize;
	pageTable ptable = allPageTable[pid];
	//�ҵ�����ҳ�� 
	//cout<<"ptable.table[0]: "<< ptable.table[0]<<endl;
	int virtual_startID = ptable.transform(pid, start, ps);
	int virtual_endID = ptable.transform(pid, end, ps);
	if (virtual_startID == -1 || virtualMemory[virtual_startID][0] < page_offset)
	{
		printf("Access memory error. Page does not exist in memory.\n");
		return "-1";
	}

	//LRU�㷨��Ҫ�ж��Ƿ���Ķ���
	else
	{
		if (replaceStrategy == 0)
		{
			ptable = FIFO(virtual_startID, ptable);
			if (virtual_startID != virtual_endID)
				ptable = FIFO(virtual_endID, ptable);
		}
		else if (replaceStrategy == 1)
		{
			ptable = LRU(virtual_startID, ptable);
			if (virtual_startID != virtual_endID)
				ptable = LRU(virtual_endID, ptable);
		}
	}
	//һ��Ҫ����ȥ����Ȼ��������� 
	allPageTable[pid] = ptable;

	int physical_start_id = ptable.find_physical(virtual_startID);
	cout << "physical start id: " << physical_start_id << endl;
	int physical_end_id = ptable.find_physical(virtual_endID);
	cout << "physical end id: " << physical_end_id << endl;
	//��������ڴ���Ķ���

	string str;
	//��ͬһҳ

	start %= pageSize;
	end %= pageSize;
	if (virtual_startID == virtual_endID)
	{
		len = end - start;
		//ljy:forkʱ���Ƹ����̵ĸĽ�����//0523zmy:�ڴ汨��ע�͵���
		//int len1 = Physical[physical_start_id].substr(start).size();
		//len = min(len, len1);
		str = Physical[physical_start_id].substr(start, len);
	}

	//��β��ҳ
	else {
		len = pageSize - page_offset;
		str = Physical[physical_start_id].substr(start, len);
		string str2 = Physical[physical_end_id].substr(0, end % pageSize);
		//cout<<str2<<endl;
		str.append(str2);
	}
	qDebug() << QString::fromStdString(Physical[physical_start_id]) << endl;
	//cout<<"Physical: "<<Physical[0]<<endl;����ǶԵ� 
	return str;

	//ֻ�ܷ��ص�һҳ�����һҳ���ٶ�Ĳ��ܷ����ˣ����߻��������Ż�
	//return Physical[physical_start_id].substr(start, len);
}

string memoryManager::watchMemory() {
	string str = "";
	//�����ڴ沿��
	str += "virtual memory usage:\n";
	for (int i = 0; i < VIRTUAL_NUMBER; i++) {
		str += "NO"; str += to_string(i); str += ": ";
		str += to_string(virtualMemory[i][0]); str += "/"; str += to_string(pageSize);
		str += "   pid: "; str += to_string(virtualMemory[i][1]);
		str += "\n";
	}
	str += "\n\n";
	//�����ڴ沿��
	str += "physical memory usage:\n";
	for (int i = 0; i < 3; i++) {
		str += "No"; str += i; str += ": ";
		str += to_string(physicalMemory[i]); str += "\n";
	}
	str += "physical memory size="; str += to_string(physicalSize);
	str += "\n\n";

	return str;
}

//�����㷨��û����
pageTable memoryManager::LRU(int vnum, pageTable ptable)
{
	Tools tool;
	int flag = tool.find_physical(-1); //��¼һ�����е�λ��

	//cout<<vnum<<endl;
	//cout<<ptable.table[vnum]<<endl;

	// �Ѿ��������ڴ��У������û�
	if (ptable.table[vnum] != -1)
	{
		schedule_queue.remove(vnum);
		schedule_queue.push_back(vnum);
	}

	//physical_Memory�п���λ��
	else if (flag != -1)
	{
		//Ӧ�ÿ���һ���û���ʱ���ǲ���Ҫ�Ѷ�����գ������ã�
		physicalMemory[flag] = vnum;
		physicalSize += virtualMemory[vnum][0];
		schedule_queue.push_back(vnum);
		ptable.modify(vnum, flag, 1);
		//�������ڴ�������ݸ��Ƶ������ڴ�
		Physical[flag] = Virtual[vnum];
		page_fault++;
	}

	//��Ҫ�û�
	else
	{
		//�û����е�һ��
		int index = tool.find_physical(schedule_queue.front()); //�˴����ؿ����滻���±�
		physicalMemory[index] = vnum;
		cout << "vnum:" << vnum << endl;
		int pid = virtualMemory[schedule_queue.front()][1];

		//�޸��ڴ�״̬
		physicalSize -= virtualMemory[schedule_queue.front()][0];
		physicalSize += virtualMemory[vnum][0];
		page_fault++;

		pageTable tmp_p = allPageTable[pid];
		tmp_p.modify(schedule_queue.front(), 0, -1);
		schedule_queue.pop_front();
		schedule_queue.push_back(vnum);
		ptable.modify(vnum, index, 1);
		Physical[index] = Virtual[vnum];
	}
	return ptable;
}


pageTable memoryManager::FIFO(int vnum, pageTable ptable)
{
	Tools tool;
	int flag = tool.find_physical(-1); //��¼һ�����е�λ��

	// �Ѿ��������ڴ��У������û�
	if (ptable.table[vnum] != -1)
	{
		printf("no need to switch page.\n");
		//Ӧ����������ҳ��
	}

	//physical_Memory�п���λ�ã�ֱ�Ӱ���
	else if (flag != -1)
	{
		//Ӧ�ÿ���һ���û���ʱ���ǲ���Ҫ�Ѷ�����գ������ã�
		physicalMemory[flag] = vnum;
		physicalSize += virtualMemory[vnum][0];
		schedule_queue.push_back(vnum);
		ptable.modify(vnum, flag, 1);
		Physical[flag] = Virtual[vnum];
		page_fault++;
	}

	//��Ҫ�û�
	else
	{
		//�û����е�һ��
		int index = tool.find_physical(schedule_queue.front()); //�˴����ؿ����滻���±�
		physicalMemory[index] = vnum;
		int pid = virtualMemory[schedule_queue.front()][1];

		//�޸��ڴ�״̬
		physicalSize -= virtualMemory[schedule_queue.front()][0];
		physicalSize += virtualMemory[vnum][0];
		page_fault++;

		pageTable tmp_p = allPageTable[pid];
		tmp_p.modify(schedule_queue.front(), 0, -1);
		schedule_queue.pop_front();
		schedule_queue.push_back(vnum);
		ptable.modify(vnum, index, 1);
		Physical[index] = Virtual[vnum];
		page_fault++;
	}
	return ptable;
}
