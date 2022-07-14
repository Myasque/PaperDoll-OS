#pragma once
#define INS_LEN 32//ָ���
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
/******************************************PROCESS.h����*******************************************/
/**************************************************************************************************/
//class INTERRUPT;
//class PCB;


enum Instruct   //ָ��
{
	Computing,  //���ʱ��
	Fork,
	NewFile,    //����ļ���
	readFile,   //����ļ���
	writeFile,  //����ļ���
	DelFile,    //����ļ���
	Access,     //����߼���ַ
	Keyboard,
	Monitor,
	Printer,
	END
};

class PROCESS
{
public:
	int pidMax;   //��һ���ɷ���Ľ���pid
	int pidRunning;   //�������еĽ���
	map<int, PCB> process; //��pidΨһ��ӦPCB
	QUEUE QueueReady;  //�������С���CPU���ȷ�ʽΪFCFS �� SJF
	MLFQUEUE MLFQueue; //�������С���CPU���ȷ�ʽΪMFLFQ
	QUEUE QueueBlock;  //��������
	INTERRUPT* myInterrupt;  //�ж�������

public:
	PROCESS() { this->pidMax = 0; this->pidRunning = -1; };
	PCB* findPCB(int pid);  //����pid�������ӦPCB
	string showPCBQueue();   //��ӡ����PCB����Ϣ

	Instruct getInstruction(string str); //�����ַ���ָ��õ�ö������
	int createProcess(int priority, string fileName);  //�������̣��ɹ�����0��ʧ�ܷ���-1
	void killProcess(int pid);   //��ֹ����
	void runProcessAll(int pid);//zmy:���������Process,�ڶ�д�ļ�ʱʹ��
	int runProcessStep(int pid);//������Process����runFileʱʹ��
	void fork(int pid);   //������������pid

	int scheduling();  //CPU���ȣ�������һ��Ҫ���е�pid
	int FCFS(QUEUE QueueReady);   //first come first served
	int SJF();    //shorest job first
	int findShortestJob(vector<int> queue);  //������ҪCPUʱ����̵Ľ���pid
	int preeSJF();  //SJF==1 && preemptive==1 && �н��̽������̬ʱʹ�ã��ж��Ƿ���ռ
	int MLFQ();   //multilevel feedback queue


	//����״̬Ǩ��
		//ע�����CPU�����㷨ѡ����MLFQ������ݽ������ȼ��ҵ���������������������
			//ע���˴� ����> ����ʱ���Ƚ��ͽ������ȼ�(0����>1, 1����>2, 2����)
		//ע�����SJF��preemptive����ÿ���н��̽������̬ʱ����Ҫ�ж��Ƿ���ռ
	void NewToReady(int pid);     //���̴����ɹ����������̬
	void ReadyToRunning(int pid); //���̱����ȣ���������̬
	void RunningToReady(int pid); //����̬ ����> ����̬
	void RunningToBlock(int pid); //����̬ ����> ����̬
	void BlockToReady(int pid);   //����̬ ����> ����̬

	//zmy:�����ж�
	int interruptTimeSlice(double timeslice, PCB* pcb);
	void interruptIO(int pid, string device);
	void interruptPageMiss(int pid, int start, int end);
	void interruptUser(PCB* pcb);
};