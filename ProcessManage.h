#pragma once
#include<time.h>
#include<vector>
#include<string>
#include<stdio.h>
#include<iostream>
#include<cstdlib>
#include<map>
//#include<QTime>
//#include<QGuiApplication>
//#include<QEventLoop>
#include <windows.h>
//#include"PROCESS.h"
#include"MemoryManage.h"
extern int isMLFQ;  //是否选择MLFQ算法，是=1，否=0
extern int isSJF;  //是否选择SJF算法，是=1，否=0
extern int isFCFS;  //是否选择FCFS算法，是=1，否=0
extern int isPreemptive;  //是否抢占，是=1，否=0，仅SJF模式下使用
extern double timeslice0;   //队列0的时间片
extern double timeslice1;  //队列1的时间片
//extern const int pageSize;  //页面大小

using namespace std;

/**************************************************************************************************/
/******************************************QUEUE.h内容*******************************************/
/**************************************************************************************************/
class QUEUE
{
private:
	int priority;  //队列优先级
public:
	double timeslice;  //时间片
	vector<int> queue;   //进程队列，存储进程pid
	int getQueueNum(); //返回队列中的进程数量
	int getPID(int pos); //根据进程处在队列中位置(从0始)返回pid
	//int shortestJob();   //遍历队列，找到最小的timeLast，返回pid
	void addProcess(int pid);  //添加进程
	void popProcess(int pid);  //删除进程
};
/**************************************************************************************************/
/******************************************MLFQUEUE.h内容*******************************************/
/**************************************************************************************************/
//extern double timeslice0;   //队列0的时间片
//extern double timeslice1;  //队列1的时间片

//多级反馈队列
//三个队列分别为队列0、队列1和队列2，对应的进程优先级分别为0、1、2（0最高）
//队列0和队列1：RR
//队列2：FCFS
class MLFQUEUE
{
private:

public:
	QUEUE queue0;
	QUEUE queue1;
	QUEUE queue2;
	int getQueueNum(int num);  //返回队列中的进程数量，参数为队列编号(取值0/1/2)
	int getPID(int queueNum, int pos); //根据进程所在队列号和处在队列中位置(从0始)返回pid
	void addProcess(int pid, int priority);  //根据进程优先级，将进程添加到queue0 || queue1 || queue2
	void popProcess(int pid, int priority);  //根据进程优先级，将进程从queue0 || queue1 || queue2中删除
};
/**************************************************************************************************/
/******************************************PCB.h内容*******************************************/
/**************************************************************************************************/

enum STATE
{
	New,
	Ready,
	Running,
	Block,
	Terminated
};
/*
extern typedef struct   //存储进程打开的文件信息
{
	string filename;  //文件名
	int start;   //文件存储在内存中的起始地址
	int end;   //文件存储在内存中的结束地址
}FL;
*/
class PCB
{
public:
	int pid;   //进程标识符
	STATE state;  //进程状态
	int size;  //进程大小
	string IR;  //指令寄存器，保存当前指令
	long PC;  //程序计数器，存放下一条指令的地址
	time_t timeArrive; //进程到达时间
	time_t timeLUCPU;  //进程最近一次使用CPU的时间
	int timeLast;   //进程需要持续时间
	int priority;  //进程优先级
	int ppid;  //父进程id
	vector<int> cpid;  //子进程id
	string bufferIO;   //存储IO交互的内容
	string nextFile;	//进程下一个要处理的文件名
	vector<FL> fileP;  //存储进程打开的文件（内存中）信息

	PCB();
	PCB(int pid, int size);
	PCB(int pid, int size, int priority);

	string outState(); //由于进程的状态定义了枚举不能打印字符串，所以用该函数打印状态
	string outPCB();  //将PCB内所有信息输出
};
/**************************************************************************************************/
/******************************************INTERRUPT.h内容*******************************************/
/**************************************************************************************************/

//extern int isMLFQ;  //是否是多级队列//注释掉了因为后面也有定义
class PROCESS;
enum InterrupType
{
	TimeSlice,
	IO,
	PageMiss,
	User
};

class INTERRUPT
{
	//PROCESS* myProcess;
public:
	int interruptTimeSlice(double timeslice, PCB* pcb);
	void interruptIO(int pid, string device);
	void interruptPageMiss(int pid, int start, int end);
	void interruptUser(PCB* pcb);
};











