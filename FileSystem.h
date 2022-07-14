#pragma 
#define READ_T 0
#define READ_E 1
#define MAX_DIR_LEN 32
#define INS_LEN 32//指令长度
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
//#include <Windows.h> //不用多线程的话可以删掉的！
#include"MemoryManage.h"

using namespace std;

/**************************************************************************************************/
/******************************************Disk.h内容**********************************************/
/**************************************************************************************************/
#define TRACK_NUM 8
#define BLOCK_NUM 32
#define BLOCK_SEQ TRACK_NUM*BLOCK_NUM
#define BLOCK_SIZE 1024

#define DISK_NO_SPACE -1
const string diskLoc = "PD_OS_FS\\disks\\";
const string fslLoc = "PD_OS_FS\\free_space_list.txt";

class BLOCK
{
public:
	int blockSeq; //块号
	string blockCon; //内容，其实应该用char[1024]限制一下长度，但我不想
	BLOCK(int blockNum)
	{
		this->blockSeq = blockNum;
		string fileName = diskLoc + "block" + to_string(blockNum) + ".txt";
		ifstream ifs(fileName);
		string buf;
		getline(ifs, buf);
		this->blockCon += buf;
		while (getline(ifs, buf))
		{
			this->blockCon += "\n" + buf;
		}
		ifs.close();
	}
	~BLOCK()
	{
		string fileName = diskLoc + "block" + to_string(this->blockSeq) + ".txt";
		ofstream ofs(fileName, ios::trunc);
		ofs << this->blockCon;
		ofs.close();
	}
};

class DISK
{
	vector<vector<BLOCK>> virtualDisk; //就是磁盘
	vector<bool> freeSpaceList; //假装这是个bit_vector，和块号对应
public:
	DISK();
	~DISK();
	int allocBlock(string context); //分配一个空闲的磁盘块并写入，返回块号
	string readBlock(int blockNum, int mode = READ_T);
	void eraseBlock(int blockNum);
	BLOCK* findBlock(int blockNum);
};

/**************************************************************************************************/
/******************************************FileControlBlock.h**************************************/
/**************************************************************************************************/
#pragma once

//using namespace std;
//#include "Disk.h"

//文件权限
#define AUTH_ALL 0
#define AUTH_NO_DEL 1
#define AUTH_READ_ONLY 2

//打印目录模式
#define THIS_DIR 0
#define CHIL_DIR 1

//索引的最大长度
#define MAX_INDEX_NUM 32

//返回值
#define SUC 0
#define R_DONE 0
#define R_TO_DONE 1

class FCB
{
public:
	string name;		//文件名，在每个目录下唯一
	char type;			//目录(.f)、程序文件(.e)、用户文件(.t)
	FCB* parent;		//指向所在目录，有且只有根目录的parent为NULL
	int size;			//文件长度，每次写操作后都需要更新
	int auth;			//权限，无限制（auth=0），不可删除（auth=1），只读（auth=2）
	vector<int> index;	//索引，第i个元素表示文件第i块地址，限制最多MAX_INDEX块
	//目录文件有这项，其他文件为空
	unordered_map<string, FCB> directoryTree;

	FCB(string fileName = "~", int fileSize = 0, char fileAuth = -1, vector<int> fileIndex = { -1 });

	int deleteFCB(string fileName);			//删除目录下的一个FCB/ok
	vector<FCB> saveDirectory();			//关闭文件系统时遍历整个目录保存/ok
	FCB* findFile(string fileName);			//在当前目录下查找/ok

	string printRoute();					//打印当前路径/ok
	vector<string> printFile(const char* fileType);//返回某种类型的所有文件/ok
	//mode = 0打印当前目录下文件，mode = 1打印所有子目录文件/ok
	string printDirectory(string dirRoute = "~", int mode = CHIL_DIR);
};

//位置指针
extern FCB* POS_POINTER;

/**************************************************************************************************/
/******************************************OpenFileTable.h*****************************************/
/**************************************************************************************************/

class OftEntry
{
	int readFlag;		//读文件标识符，计数信号量，标识正在读的进程数量
	int writeFlag;		//写文件标识符，二元信号量，一次只允许一个进程写入
public:
	FCB* file;			//文件控制块副本，包含文件信息

	OftEntry(FCB* fcbFile)
	{
		this->readFlag = 0;
		this->writeFlag = 0;
		this->file = fcbFile;
	}

	/*
	* 修改信号量的函数我写得比较偷懒，没有做防差错处理……其实还不如直接不封装呢
	* 不过优点大概是，如果以后要修改这块的操作，写得更优美的话，函数方便改
	*/
	void R_occuppy() { this->readFlag++; }
	void R_release() { this->readFlag--; }
	void W_occuppy() { this->writeFlag = 1; }
	void W_release() { this->writeFlag = 0; }
	//返回信号量
	int getReadFlag() { return readFlag; }
	int getWriteFlag() { return writeFlag; }
};

class OFT
{
public:
	vector<OftEntry> OfTable;
	OftEntry* searchOft(string fileName); //根据文件名在OFT中查找
	int deleteOft(string fileName);
};



/**************************************************************************************************/
/******************************************FileSystem.h**************************************/
/**************************************************************************************************/
//#include "test.h" //合并时请替换！！！！！
//#include "OpenFileTable.h"

#define WRITE_HEAD 0
#define WRITE_TAIL 1
#define MAX_NAME_LEN 19

//返回值
#define SUC 0
#define ERR -1
#define C_ERR_DUP_NAME -1
#define C_ERR_ILL_NAME -2
#define D_ERR_LACK_AUTH -1
#define D_ERR_IS_OCCUPY -2
#define D_ERR_NO_FILE -3
#define R_ERR_IS_OCCUPY -1
#define W_ERR_IS_OCCUPY -1
#define W_ERR_NO_SPACE -2
#define W_ERR_LACK_AUTH -3

//在电脑上的保存位置，如有需要请替换！！！！！
const string dirLoc = "PD_OS_FS\\directory.txt";

class FileSystem
{
private:
	FCB myDir;
	OFT myOft;
	DISK myDisk;
	//memoryManager myMm;
	void releaseDisk(FCB* dFile);	//删除文件的工具
	string readDisk(FCB* rFile);		//读文件工具
public:
	int IRnum = 0;//zmy:im sorry!
	//QTreeWidget treeWidget;//FileTree尝试
	FileSystem();
	~FileSystem();

	//这三个功能的实现其实在FCB，但是为了调用方便，写成这个样子感觉更合适/ok
	string printRoute() { return POS_POINTER->printRoute(); }
	vector<string> printFile(const char* fileType) { return POS_POINTER->printFile(fileType); }
	string printDirectory() { return this->myDir.printDirectory(); }

	int changeRoute(string route);			//修改POS_POINTER指针/ok
	int creatFile(string fileName);			//创建文件/ok
	int deleteFile(string fileName);		//删除文件/ok
	FL readFile(string fileName, int pid);	//读文件
	//写文件，mode = WRITE_HEAD覆写，mode = WRITE_TAIL续写
	int writeFile(string fileName, string writeObj, int mode, int pid = 0);
};



