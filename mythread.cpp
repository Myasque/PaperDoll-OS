#include "mythread.h"
#include"PROCESS.h"
#include <QDebug>
#include <QMutex>

extern PROCESS myPS;
extern int isPreemptive;

MyThread::MyThread()
{
	isStop = false;
}

void MyThread::closeThread()
{
	isStop = true;
}

void MyThread::run()
{
	int isEnd = 0;
	while (1)
	{
		if (isStop) return;

		if (myPS.pidRunning == -1) myPS.scheduling();//shce

		if (myPS.pidRunning != -1) {
			time_t timep; time(&timep);
			myPS.findPCB(myPS.pidRunning)->timeLUCPU = timep;

			isEnd = myPS.runProcessStep(myPS.pidRunning);

			sleep(2);

			qDebug() << "myThread::pidRunning=" << myPS.pidRunning;
			if (myPS.pidRunning != -1) {
				PCB* pcb = myPS.findPCB(myPS.pidRunning);
				while (isEnd == 0 && pcb->state == Running && myPS.pidRunning != -1) {//还没有结束
					isEnd = myPS.runProcessStep(myPS.pidRunning);
					//qDebug() << "runProcess is End ? :" << isEnd;
					sleep(2);//每执行一条指令耗时2s
				}
			}


		}

		sleep(1);
	}
}