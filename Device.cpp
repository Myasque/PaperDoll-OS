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
	//����е������豸
	initDevice("keyboard", 0);
	initDevice("monitor", 0);
	initDevice("printer", 0);
}


void DeviceSystem::createDevice(string deviceName) {
	//����豸
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
	//�����豸
	for (int i = 0; i < deviceNum; i++) {
		if (devices[i].deviceName == device) {//�ҵ�device��i
			if (requestNum[i] < REQUESTMAXNUM) {//�豸����������������ֵ
				devices[i].request[requestNum[i]].pid = id;//���µ����豸���������
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


int DeviceSystem::FCFS(string device) {//���device[2] keyboard���д���
	int tmp;//�洢pid
	//�����ȷ���
	devices[2].isBusy = 1;
	qDebug() << "device[2].isbusy being set to 1";
	//ms����
	//Sleep(devices[2].request[0].ioTime / 1000);//�߳�sleep

	tmp = devices[2].request[0].pid;
	//�൱�ڶ���popһ�¸ղŴ������request
	for (int j = 0; j < requestNum[2] - 1; j++) {
		devices[2].request[j].pid = devices[2].request[j + 1].pid;
		devices[2].request[j].ioTime = devices[2].request[j + 1].ioTime;
	}
	requestNum[2]--;

	return tmp;
}

//�ж����е��豸�л���û��request������о�һ���豸һ���豸ȥfcfs
//������ҪFCFS���豸
int DeviceSystem::deviceWork() {
	//ѭ������
	int total = 0;
	for (int i = 0; i < deviceNum; i++) { total += requestNum[i]; }
	if (total > 0) {
		return 2;//����keyboard
	}
	return -1;
}

