#pragma once
#define VIRTUAL_NUMBER 16
#define PHYSICAL_NUMBER 3
#include<iostream>
#include<iostream>
#include<string>
#include<algorithm>
#include<map>
#include<list>
#include<QDebug>

using namespace std;

typedef struct file_location {
	string filename;
	int start;
	int end;
}FL;

class Tools
{
public:
	int find_physical(int elem);
};

class pageTable
{
public:

	typedef std::map<int, int> TABLE;
	TABLE table;
	int max_adress = 0;
	//allocated virtual page number
	void insert_virtual(int pageNumber);
	void delete_page(int pageNumber);
	//在pageTable中的位置
	int transform(int pid, int address, int pageSize);
	// when the virtual page being schedule in/out the physical memory
	void modify(int vNum, int pNum, int valid);
	int find_physical(int index);
};


class memoryManager
{
public:
	typedef std::map<int, pageTable> PAGETABLE; //<pid, pageTable>每一个进程对应一个pageTable
	PAGETABLE allPageTable;
	void Initial();
	int pageAlloc(int pid, int size);
	int pageFree(int pid);
	FL writeVirtual(int pid, string context);
	int checkPhysical(int pid, int start, int end);
	int pageIO(int pid, int start, int end);
	string pageAccess(int pid, int start, int end);
	string watchMemory();
	//void watch_virtual_memory();
	//void watch_physical_memory();
	//两个算法还没检验
	pageTable LRU(int vnum, pageTable ptable);
	pageTable FIFO(int vnum, pageTable ptable);
};
