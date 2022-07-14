#pragma once
#include<iostream>
#include<string>
#include<string.h>
#include<iomanip>
#include<qDebug>
using namespace std;

#define REQUESTMAXNUM 12
#define DEVICEMAXNUM 12
extern int deviceNum;
extern int requestNum[DEVICEMAXNUM];//ÿ���豸��������

extern string deviceBuffer;

typedef struct deviceRequest {
	int pid;//����ID(-1��ʾ��)
	//string data;//�������ݣ����У�
	double ioTime;//��ʹ�ø��豸��ʱ�������У�
}Request;

typedef struct Device {
	string deviceName;//�豸����
	int tansmitRate;//�������ʣ����У�
	int isBusy;//�Ƿ�ռ��
	Request request[REQUESTMAXNUM];//���豸���������
}Device;


class DeviceSystem :public QObject
{
	Q_OBJECT
public:
	Device devices[DEVICEMAXNUM];
	string deviceState = "deviceName\t\ttransmitRate\t\tisBusy\t\tpid\nkeyboard\t\t0\t\t0\t\tnull\nmonitor\t\t 0\t\t0\t\tnull\nprinter\t\t0\t\t0\t\tnull\n";
	DeviceSystem() { initIO(); };//zmy:���캯��
	void initIO();
	int deviceWork();//������ҪFCFS���豸��
	void createDevice(string deviceName);
	string showDevices();
	void requestDevice(int id, string device, double time);
	void initDevice(string name, double transRate);
	int FCFS(string device);
};



