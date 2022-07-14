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
extern int requestNum[DEVICEMAXNUM];//每个设备的请求数

extern string deviceBuffer;

typedef struct deviceRequest {
	int pid;//进程ID(-1表示无)
	//string data;//数据内容（若有）
	double ioTime;//需使用该设备的时长（若有）
}Request;

typedef struct Device {
	string deviceName;//设备名称
	int tansmitRate;//传输速率（若有）
	int isBusy;//是否被占用
	Request request[REQUESTMAXNUM];//该设备的请求队列
}Device;


class DeviceSystem :public QObject
{
	Q_OBJECT
public:
	Device devices[DEVICEMAXNUM];
	string deviceState = "deviceName\t\ttransmitRate\t\tisBusy\t\tpid\nkeyboard\t\t0\t\t0\t\tnull\nmonitor\t\t 0\t\t0\t\tnull\nprinter\t\t0\t\t0\t\tnull\n";
	DeviceSystem() { initIO(); };//zmy:构造函数
	void initIO();
	int deviceWork();//返回需要FCFS的设备号
	void createDevice(string deviceName);
	string showDevices();
	void requestDevice(int id, string device, double time);
	void initDevice(string name, double transRate);
	int FCFS(string device);
};



