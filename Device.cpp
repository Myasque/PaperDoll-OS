#include<windows.h>
#include "device.h"

int deviceNum = 0;
int requestNum[DEVICEMAXNUM] = { 0 };

string deviceBuffer;


void DeviceSystem::initDevice(string name, double transRate) {
	devices[deviceNum].deviceName = name;
	devices[deviceNum].tansmitRate = transRate;
	devices[deviceNum].isBusy = 0;
	for (int i = 0; i < REQUESTMAXNUM; i++) {
		devices[deviceNum].request[i].pid = -1;
		devices[deviceNum].request[i].ioTime = 0;
	}
	deviceNum++;
}


void DeviceSystem::initIO() {
	//最初有的三个设备
	initDevice("keyboard", 0);
	initDevice("monitor", 0);
	initDevice("printer", 0);
}


void DeviceSystem::createDevice(string deviceName) {
	//添加设备
	int rate = 5;
	if (deviceNum == DEVICEMAXNUM) {
		qDebug() << "there is no room for a new device" << endl;
		return;
	}
	else {
		devices[deviceNum].deviceName = deviceName;
		devices[deviceNum].tansmitRate = rate;
		devices[deviceNum].isBusy = 0;

		for (int i = 0; i < REQUESTMAXNUM; i++) {
			devices[deviceNum].request[i].pid = -1;
			//devices[deviceNum].request[i].data = "\0";
			devices[deviceNum].request[i].ioTime = 0;
		}
		deviceNum++;
		qDebug() << "create successfully" << endl;
	}
}

string DeviceSystem::showDevices() {
	deviceState.assign("deviceName\t\ttransmitRate\tisBusy\t\tpid\n");
	for (int k = 0; k < deviceNum; k++) {
		deviceState.append(devices[k].deviceName);
		deviceState.append("\t\t");
		deviceState.append(to_string(devices[k].tansmitRate));
		deviceState.append("\t\t");
		deviceState.append(to_string(devices[k].isBusy));
		deviceState.append("\t\t");
		if (devices[k].isBusy == 1) { deviceState.append(to_string(devices[k].request[0].pid)); }
		else { deviceState.append("null"); }
		deviceState.append("\n");
	}
	return deviceState;
}


void DeviceSystem::requestDevice(int id, string device, double time) {
	//申请设备
	for (int i = 0; i < deviceNum; i++) {
		if (devices[i].deviceName == device) {//找到device：i
			if (requestNum[i] < REQUESTMAXNUM) {//设备的请求队列少于最大值
				devices[i].request[requestNum[i]].pid = id;//更新到该设备的请求队列
				devices[i].request[requestNum[i]].ioTime = time;
				requestNum[i]++;
				qDebug() << id << " request " << QString::fromStdString(device) << " successfully" << endl;
			}
			else {
				qDebug() << "this device's queue is long,please request later." << endl;
			}
			break;
		}
		if (i == deviceNum - 1) {
			qDebug() << "this device " << QString::fromStdString(device) << " doesn't exist." << endl;
		}
	}
}


int DeviceSystem::FCFS(string device) {//针对device[2] keyboard进行处理
	int tmp;//存储pid
	//先来先服务
	devices[2].isBusy = 1;
	qDebug() << "device[2].isbusy being set to 1";
	//ms级别
	//Sleep(devices[2].request[0].ioTime / 1000);//线程sleep

	tmp = devices[2].request[0].pid;
	//相当于队列pop一下刚才处理过的request
	for (int j = 0; j < requestNum[2] - 1; j++) {
		devices[2].request[j].pid = devices[2].request[j + 1].pid;
		devices[2].request[j].ioTime = devices[2].request[j + 1].ioTime;
	}
	requestNum[2]--;

	return tmp;
}

//判断所有的设备中还有没有request，如果有就一个设备一个设备去fcfs
//返回需要FCFS的设备
int DeviceSystem::deviceWork() {
	//循环调用
	int total = 0;
	for (int i = 0; i < deviceNum; i++) { total += requestNum[i]; }
	if (total > 0) {
		return 2;//返回keyboard
	}
	return -1;
}

