# include "MemoryManage.h"

//内存部分全局变量
const char mode = 'p'; //分页mode=p 连续mode=c
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
	int idx = address / pageSize; //在进程的第几页（进程相对位置）
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
		return tmptable[idx]; //返回虚拟页号
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
			if (allPageTable.count(pid) > 0) //该进程已经在虚拟内存中有部分页了，再加一个新的就行
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
					//释放实际上有内容的物理内存
					Physical[j] = "*";
					//更改队列
					schedule_queue.remove(i);
				}
			}
			//free it from virtual memory
			allocated -= virtualMemory[i][0];
			virtualMemory[i][0] = 0;
			virtualMemory[i][1] = -1;
			virtualMemory[i][2] = 0;
			//释放实际上有内容的虚拟内存
			Virtual[i] = "*";
		}
	}
	if (status == 0)
	{
		printf("Relase memory failed. Process does not exist in memory.\n");
		return -1;
	}
	allPageTable.erase(pid); //杀死进程 
	printf("Release memory pid[%d].\n", pid);
	return 0;
}

FL memoryManager::writeVirtual(int pid, string context)
{
	int i = 0;
	FL file_location;
	int len = context.length();
	//zmy:如果是空文件
	//if (len == 0) { file_location.end = 0; file_location.start = 0; return file_location; }
	pageTable ptable = allPageTable[pid];
	map<int, int> Table = ptable.table; //只用于遍历，不用于修改 
	// 保存所有虚拟页号
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
			//只要修改，就从物理内存里删除
			if (!schedule_queue.empty()) {
				list<int>::iterator it = find(schedule_queue.begin(), schedule_queue.end(), tmp); // 查找schedule-queue中是否有要写入的页
				if (it != schedule_queue.end()) // 找到了
				{
					physicalMemory[ptable.table[tmp]] = -1;
					Physical[ptable.table[tmp]] = "*";
					ptable.table[tmp] = -1;
					allPageTable[pid] = ptable;
					schedule_queue.remove(ptable.table[tmp]);
				}
			}
			//第一次发现可用页以后就不再往后加begin了 
			flag = 1;
			cout << "虚拟页号: " << tmp << endl;

			if (Virtual[tmp] == "*")	Virtual[tmp].erase(0, 1);
			//追加在当前存在了的 后面 
			writeLen = min(len, virtualMemory[tmp][0] - virtualMemory[tmp][2]);
			cout << "写入长度: " << writeLen << endl;
			Virtual[tmp].append(context, 0, writeLen);
			virtualMemory[tmp][2] += writeLen;
			qDebug() << "LOOOOK AT the memory,tmp=" << QString::number(tmp) << "and the Virtual[tmp]is";
			qDebug() << QString::fromStdString(Virtual[tmp]);
			//删除已写入部分
			context.erase(0, writeLen);
			cout << "剩余内容: " << context << endl;
			len -= writeLen;
			cout << "剩余长度: " << len << "\n" << endl;
			end += writeLen;
		}
		index.pop_front();
		writeLen = 0;
	}

	file_location.start = begin;
	file_location.end = end - 1;

	if (len > 0) {
		printf("空间不够，没完全写入\n");
		return file_location;
	}
	end += len;//多加的地方倒回去

	qDebug() << "The Memory put" << QString::fromStdString(context) << "in{start,end}:";
	qDebug() << QString::number(file_location.start) << " , " << QString::number(file_location.end);
	return file_location;

}


int memoryManager::checkPhysical(int pid, int start, int end)
{
	int page_offset = start % pageSize;
	int ps = pageSize;
	pageTable ptable = allPageTable[pid];
	//找到虚拟页号 
	//cout<<"ptable.table[0]: "<< ptable.table[0]<<endl;
	int virtual_startID = ptable.transform(pid, start, ps);
	int virtual_endID = ptable.transform(pid, end, ps);

	//不缺页中断
	if (ptable.table[virtual_startID] != -1 && ptable.table[virtual_startID] != -1)
	{
		return 0;
	}
	//缺页中断
	else
	{
		return -1;
	}
}

int memoryManager::pageIO(int pid, int start, int end) {
	int page_offset = start % pageSize;
	int ps = pageSize;
	pageTable ptable = allPageTable[pid];
	//找到虚拟页号 
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
	//找到虚拟页号 
	//cout<<"ptable.table[0]: "<< ptable.table[0]<<endl;
	int virtual_startID = ptable.transform(pid, start, ps);
	int virtual_endID = ptable.transform(pid, end, ps);
	if (virtual_startID == -1 || virtualMemory[virtual_startID][0] < page_offset)
	{
		printf("Access memory error. Page does not exist in memory.\n");
		return "-1";
	}

	//LRU算法需要判断是否更改队列
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
	//一定要赋回去，不然会错！！！！ 
	allPageTable[pid] = ptable;

	int physical_start_id = ptable.find_physical(virtual_startID);
	cout << "physical start id: " << physical_start_id << endl;
	int physical_end_id = ptable.find_physical(virtual_endID);
	cout << "physical end id: " << physical_end_id << endl;
	//输出物理内存里的东西

	string str;
	//在同一页

	start %= pageSize;
	end %= pageSize;
	if (virtual_startID == virtual_endID)
	{
		len = end - start;
		//ljy:fork时复制父进程的改进尝试//0523zmy:内存报错注释掉了
		//int len1 = Physical[physical_start_id].substr(start).size();
		//len = min(len, len1);
		str = Physical[physical_start_id].substr(start, len);
	}

	//首尾两页
	else {
		len = pageSize - page_offset;
		str = Physical[physical_start_id].substr(start, len);
		string str2 = Physical[physical_end_id].substr(0, end % pageSize);
		//cout<<str2<<endl;
		str.append(str2);
	}
	qDebug() << QString::fromStdString(Physical[physical_start_id]) << endl;
	//cout<<"Physical: "<<Physical[0]<<endl;这句是对的 
	return str;

	//只能返回第一页和最后一页，再多的不能返回了，或者还可以再优化
	//return Physical[physical_start_id].substr(start, len);
}

string memoryManager::watchMemory() {
	string str = "";
	//虚拟内存部分
	str += "virtual memory usage:\n";
	for (int i = 0; i < VIRTUAL_NUMBER; i++) {
		str += "NO"; str += to_string(i); str += ": ";
		str += to_string(virtualMemory[i][0]); str += "/"; str += to_string(pageSize);
		str += "   pid: "; str += to_string(virtualMemory[i][1]);
		str += "\n";
	}
	str += "\n\n";
	//物理内存部分
	str += "physical memory usage:\n";
	for (int i = 0; i < 3; i++) {
		str += "No"; str += i; str += ": ";
		str += to_string(physicalMemory[i]); str += "\n";
	}
	str += "physical memory size="; str += to_string(physicalSize);
	str += "\n\n";

	return str;
}

//两个算法还没检验
pageTable memoryManager::LRU(int vnum, pageTable ptable)
{
	Tools tool;
	int flag = tool.find_physical(-1); //记录一个空闲的位置

	//cout<<vnum<<endl;
	//cout<<ptable.table[vnum]<<endl;

	// 已经在物理内存中，不用置换
	if (ptable.table[vnum] != -1)
	{
		schedule_queue.remove(vnum);
		schedule_queue.push_back(vnum);
	}

	//physical_Memory有空闲位置
	else if (flag != -1)
	{
		//应该考虑一下置换的时候是不是要把队列清空（好像不用）
		physicalMemory[flag] = vnum;
		physicalSize += virtualMemory[vnum][0];
		schedule_queue.push_back(vnum);
		ptable.modify(vnum, flag, 1);
		//将虚拟内存里的内容复制到物理内存
		Physical[flag] = Virtual[vnum];
		page_fault++;
	}

	//需要置换
	else
	{
		//置换队列第一个
		int index = tool.find_physical(schedule_queue.front()); //此处返回可以替换的下标
		physicalMemory[index] = vnum;
		cout << "vnum:" << vnum << endl;
		int pid = virtualMemory[schedule_queue.front()][1];

		//修改内存状态
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
	int flag = tool.find_physical(-1); //记录一个空闲的位置

	// 已经在物理内存中，不用置换
	if (ptable.table[vnum] != -1)
	{
		printf("no need to switch page.\n");
		//应当返回物理页号
	}

	//physical_Memory有空闲位置，直接安排
	else if (flag != -1)
	{
		//应该考虑一下置换的时候是不是要把队列清空（好像不用）
		physicalMemory[flag] = vnum;
		physicalSize += virtualMemory[vnum][0];
		schedule_queue.push_back(vnum);
		ptable.modify(vnum, flag, 1);
		Physical[flag] = Virtual[vnum];
		page_fault++;
	}

	//需要置换
	else
	{
		//置换队列第一个
		int index = tool.find_physical(schedule_queue.front()); //此处返回可以替换的下标
		physicalMemory[index] = vnum;
		int pid = virtualMemory[schedule_queue.front()][1];

		//修改内存状态
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
