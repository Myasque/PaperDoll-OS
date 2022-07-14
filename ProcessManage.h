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
extern int isMLFQ;  //�Ƿ�ѡ��MLFQ�㷨����=1����=0
extern int isSJF;  //�Ƿ�ѡ��SJF�㷨����=1����=0
extern int isFCFS;  //�Ƿ�ѡ��FCFS�㷨����=1����=0
extern int isPreemptive;  //�Ƿ���ռ����=1����=0����SJFģʽ��ʹ��
extern double timeslice0;   //����0��ʱ��Ƭ
extern double timeslice1;  //����1��ʱ��Ƭ
//extern const int pageSize;  //ҳ���С

using namespace std;

/**************************************************************************************************/
/******************************************QUEUE.h����*******************************************/
/**************************************************************************************************/
class QUEUE
{
private:
	int priority;  //�������ȼ�
public:
	double timeslice;  //ʱ��Ƭ
	vector<int> queue;   //���̶��У��洢����pid
	int getQueueNum(); //���ض����еĽ�������
	int getPID(int pos); //���ݽ��̴��ڶ�����λ��(��0ʼ)����pid
	//int shortestJob();   //�������У��ҵ���С��timeLast������pid
	void addProcess(int pid);  //��ӽ���
	void popProcess(int pid);  //ɾ������
};
/**************************************************************************************************/
/******************************************MLFQUEUE.h����*******************************************/
/**************************************************************************************************/
//extern double timeslice0;   //����0��ʱ��Ƭ
//extern double timeslice1;  //����1��ʱ��Ƭ

//�༶��������
//�������зֱ�Ϊ����0������1�Ͷ���2����Ӧ�Ľ������ȼ��ֱ�Ϊ0��1��2��0��ߣ�
//����0�Ͷ���1��RR
//����2��FCFS
class MLFQUEUE
{
private:

public:
	QUEUE queue0;
	QUEUE queue1;
	QUEUE queue2;
	int getQueueNum(int num);  //���ض����еĽ�������������Ϊ���б��(ȡֵ0/1/2)
	int getPID(int queueNum, int pos); //���ݽ������ڶ��кźʹ��ڶ�����λ��(��0ʼ)����pid
	void addProcess(int pid, int priority);  //���ݽ������ȼ�����������ӵ�queue0 || queue1 || queue2
	void popProcess(int pid, int priority);  //���ݽ������ȼ��������̴�queue0 || queue1 || queue2��ɾ��
};
/**************************************************************************************************/
/******************************************PCB.h����*******************************************/
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
extern typedef struct   //�洢���̴򿪵��ļ���Ϣ
{
	string filename;  //�ļ���
	int start;   //�ļ��洢���ڴ��е���ʼ��ַ
	int end;   //�ļ��洢���ڴ��еĽ�����ַ
}FL;
*/
class PCB
{
public:
	int pid;   //���̱�ʶ��
	STATE state;  //����״̬
	int size;  //���̴�С
	string IR;  //ָ��Ĵ��������浱ǰָ��
	long PC;  //����������������һ��ָ��ĵ�ַ
	time_t timeArrive; //���̵���ʱ��
	time_t timeLUCPU;  //�������һ��ʹ��CPU��ʱ��
	int timeLast;   //������Ҫ����ʱ��
	int priority;  //�������ȼ�
	int ppid;  //������id
	vector<int> cpid;  //�ӽ���id
	string bufferIO;   //�洢IO����������
	string nextFile;	//������һ��Ҫ������ļ���
	vector<FL> fileP;  //�洢���̴򿪵��ļ����ڴ��У���Ϣ

	PCB();
	PCB(int pid, int size);
	PCB(int pid, int size, int priority);

	string outState(); //���ڽ��̵�״̬������ö�ٲ��ܴ�ӡ�ַ����������øú�����ӡ״̬
	string outPCB();  //��PCB��������Ϣ���
};
/**************************************************************************************************/
/******************************************INTERRUPT.h����*******************************************/
/**************************************************************************************************/

//extern int isMLFQ;  //�Ƿ��Ƕ༶����//ע�͵�����Ϊ����Ҳ�ж���
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











