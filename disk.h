#pragma once
#include<iostream>
#include<string>
#include<string.h>
#include<iomanip>
#include <cstdlib>
#include<qDebug>
#include<ctime>

using namespace std;

class Disk {
private:
	int trackNum;//�ܴŵ���
public:
	int output[20];
	int outputNum;
	int seekQueue[20];
	int curHead;//��¼��ǰ��ͷλ��
	int seekQueueNum;
	string showDisk();
	void initDisk();
	void seekPath(int type);
	void FCFS();
	void SSTF();
	void SCAN();
	void C_SCAN();
	void LOOK();
	void C_LOOK();
};

//Disk disk;


// void show();

