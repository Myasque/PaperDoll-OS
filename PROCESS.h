#pragma once
#define INS_LEN 32//指令长度
#include<time.h>
#include<vector>
#include<string>
#include<stdio.h>
#include<iostream>
#include<cstdlib>
#include<map>
#include <windows.h>
#include<QTime>
//#include<QTimer>
#include<QString>
#include <QApplication>
#include"Device.h"
#include"MemoryManage.h"
#include"ProcessManage.h"
#include"FileSystem.h"

using namespace std;


/**************************************************************************************************/
/******************************************PROCESS.h内容*******************************************/
/**************************************************************************************************/
//class INTERRUPT;
//class PCB;


enum Instruct   //指令
{
	Computing,  //后跟时间
	Fork,
	NewFile,    //后跟文件名
	readFile,   //后跟文件名
	writeFile,  //后跟文件名
	DelFile,    //后跟文件名
	Access,     //后跟逻辑地址
	Keyboard,
	Monitor,
	Printer,
	END
};

class PROCESS
{
public:
	int pidMax;   //下一个可分配的进程pid
	int pidRunning;   //正在运行的进程
	map<int, PCB> process; //用pid唯一对应PCB
	QUEUE QueueReady;  //就绪队列——CPU调度方式为FCFS 或 SJF
	MLFQUEUE MLFQueue; //就绪队列——CPU调度方式为MFLFQ
	QUEUE QueueBlock;  //阻塞队列
	INTERRUPT* myInterrupt;  //中断向量表

public:
	PROCESS() { this->pidMax = 0; this->pidRunning = -1; };
	PCB* findPCB(int pid);  //根据pid返回其对应PCB
	string showPCBQueue();   //打印所有PCB的信息

	Instruct getInstruction(string str); //分析字符串指令，得到枚举类型
	int createProcess(int priority, string fileName);  //创建进程，成功返回0，失败返回-1
	void killProcess(int pid);   //终止进程
	void runProcessAll(int pid);//zmy:运行整体的Process,在读写文件时使用
	int runProcessStep(int pid);//逐步运行Process，在runFile时使用
	void fork(int pid);   //参数：父进程pid

	int scheduling();  //CPU调度，返回下一个要运行的pid
	int FCFS(QUEUE QueueReady);   //first come first served
	int SJF();    //shorest job first
	int findShortestJob(vector<int> queue);  //返回需要CPU时间最短的进程pid
	int preeSJF();  //SJF==1 && preemptive==1 && 有进程进入就绪态时使用，判断是否抢占
	int MLFQ();   //multilevel feedback queue


	//进程状态迁移
		//注：如果CPU调度算法选择了MLFQ，则根据进程优先级找到进程所在三级就绪队列
			//注：运次 ——> 就绪时，先降低进程优先级(0——>1, 1——>2, 2不变)
		//注：如果SJF且preemptive，则每次有进程进入就绪态时，都要判断是否抢占
	void NewToReady(int pid);     //进程创建成功，进入就绪态
	void ReadyToRunning(int pid); //进程被调度，进入运行态
	void RunningToReady(int pid); //运行态 ——> 就绪态
	void RunningToBlock(int pid); //运行态 ——> 阻塞态
	void BlockToReady(int pid);   //阻塞态 ——> 就绪态

	//zmy:进程中断
	int interruptTimeSlice(double timeslice, PCB* pcb);
	void interruptIO(int pid, string device);
	void interruptPageMiss(int pid, int start, int end);
	void interruptUser(PCB* pcb);
};