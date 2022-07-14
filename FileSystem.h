#pragma 
#define READ_T 0
#define READ_E 1
#define MAX_DIR_LEN 32
#define INS_LEN 32//ָ���
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
//#include <Windows.h> //���ö��̵߳Ļ�����ɾ���ģ�
#include"MemoryManage.h"

using namespace std;

/**************************************************************************************************/
/******************************************Disk.h����**********************************************/
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
	int blockSeq; //���
	string blockCon; //���ݣ���ʵӦ����char[1024]����һ�³��ȣ����Ҳ���
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
	vector<vector<BLOCK>> virtualDisk; //���Ǵ���
	vector<bool> freeSpaceList; //��װ���Ǹ�bit_vector���Ϳ�Ŷ�Ӧ
public:
	DISK();
	~DISK();
	int allocBlock(string context); //����һ�����еĴ��̿鲢д�룬���ؿ��
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

//�ļ�Ȩ��
#define AUTH_ALL 0
#define AUTH_NO_DEL 1
#define AUTH_READ_ONLY 2

//��ӡĿ¼ģʽ
#define THIS_DIR 0
#define CHIL_DIR 1

//��������󳤶�
#define MAX_INDEX_NUM 32

//����ֵ
#define SUC 0
#define R_DONE 0
#define R_TO_DONE 1

class FCB
{
public:
	string name;		//�ļ�������ÿ��Ŀ¼��Ψһ
	char type;			//Ŀ¼(.f)�������ļ�(.e)���û��ļ�(.t)
	FCB* parent;		//ָ������Ŀ¼������ֻ�и�Ŀ¼��parentΪNULL
	int size;			//�ļ����ȣ�ÿ��д��������Ҫ����
	int auth;			//Ȩ�ޣ������ƣ�auth=0��������ɾ����auth=1����ֻ����auth=2��
	vector<int> index;	//��������i��Ԫ�ر�ʾ�ļ���i���ַ���������MAX_INDEX��
	//Ŀ¼�ļ�����������ļ�Ϊ��
	unordered_map<string, FCB> directoryTree;

	FCB(string fileName = "~", int fileSize = 0, char fileAuth = -1, vector<int> fileIndex = { -1 });

	int deleteFCB(string fileName);			//ɾ��Ŀ¼�µ�һ��FCB/ok
	vector<FCB> saveDirectory();			//�ر��ļ�ϵͳʱ��������Ŀ¼����/ok
	FCB* findFile(string fileName);			//�ڵ�ǰĿ¼�²���/ok

	string printRoute();					//��ӡ��ǰ·��/ok
	vector<string> printFile(const char* fileType);//����ĳ�����͵������ļ�/ok
	//mode = 0��ӡ��ǰĿ¼���ļ���mode = 1��ӡ������Ŀ¼�ļ�/ok
	string printDirectory(string dirRoute = "~", int mode = CHIL_DIR);
};

//λ��ָ��
extern FCB* POS_POINTER;

/**************************************************************************************************/
/******************************************OpenFileTable.h*****************************************/
/**************************************************************************************************/

class OftEntry
{
	int readFlag;		//���ļ���ʶ���������ź�������ʶ���ڶ��Ľ�������
	int writeFlag;		//д�ļ���ʶ������Ԫ�ź�����һ��ֻ����һ������д��
public:
	FCB* file;			//�ļ����ƿ鸱���������ļ���Ϣ

	OftEntry(FCB* fcbFile)
	{
		this->readFlag = 0;
		this->writeFlag = 0;
		this->file = fcbFile;
	}

	/*
	* �޸��ź����ĺ�����д�ñȽ�͵����û���������������ʵ������ֱ�Ӳ���װ��
	* �����ŵ����ǣ�����Ժ�Ҫ�޸����Ĳ�����д�ø������Ļ������������
	*/
	void R_occuppy() { this->readFlag++; }
	void R_release() { this->readFlag--; }
	void W_occuppy() { this->writeFlag = 1; }
	void W_release() { this->writeFlag = 0; }
	//�����ź���
	int getReadFlag() { return readFlag; }
	int getWriteFlag() { return writeFlag; }
};

class OFT
{
public:
	vector<OftEntry> OfTable;
	OftEntry* searchOft(string fileName); //�����ļ�����OFT�в���
	int deleteOft(string fileName);
};



/**************************************************************************************************/
/******************************************FileSystem.h**************************************/
/**************************************************************************************************/
//#include "test.h" //�ϲ�ʱ���滻����������
//#include "OpenFileTable.h"

#define WRITE_HEAD 0
#define WRITE_TAIL 1
#define MAX_NAME_LEN 19

//����ֵ
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

//�ڵ����ϵı���λ�ã�������Ҫ���滻����������
const string dirLoc = "PD_OS_FS\\directory.txt";

class FileSystem
{
private:
	FCB myDir;
	OFT myOft;
	DISK myDisk;
	//memoryManager myMm;
	void releaseDisk(FCB* dFile);	//ɾ���ļ��Ĺ���
	string readDisk(FCB* rFile);		//���ļ�����
public:
	int IRnum = 0;//zmy:im sorry!
	//QTreeWidget treeWidget;//FileTree����
	FileSystem();
	~FileSystem();

	//���������ܵ�ʵ����ʵ��FCB������Ϊ�˵��÷��㣬д��������Ӹо�������/ok
	string printRoute() { return POS_POINTER->printRoute(); }
	vector<string> printFile(const char* fileType) { return POS_POINTER->printFile(fileType); }
	string printDirectory() { return this->myDir.printDirectory(); }

	int changeRoute(string route);			//�޸�POS_POINTERָ��/ok
	int creatFile(string fileName);			//�����ļ�/ok
	int deleteFile(string fileName);		//ɾ���ļ�/ok
	FL readFile(string fileName, int pid);	//���ļ�
	//д�ļ���mode = WRITE_HEAD��д��mode = WRITE_TAIL��д
	int writeFile(string fileName, string writeObj, int mode, int pid = 0);
};



