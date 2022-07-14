#include "threadDevice.h"
#include"PROCESS.h"
#include <QDebug>
#include <QMutex>

extern DeviceSystem myDeviceSystem;
extern PROCESS myPS;


ThreadDevice::ThreadDevice()
{
	isStop = false;
}

void ThreadDevice::closeThread()
{
	isStop = true;
}

void ThreadDevice::run()
{

	while (1)
	{

		int deviceToWork = myDeviceSystem.deviceWork();//判断是否有要处理的请求
		qDebug() << "deviceSystem deviceWork()ed!=" << deviceToWork;
		while (deviceToWork == 2) {
			int pid;
			//执行该device的第一个
			pid = myDeviceSystem.FCFS(myDeviceSystem.devices[deviceToWork].deviceName);
			qDebug() << "FCFS pid:" << pid;

			//myPS.RunningToBlock(pid);
			sleep(5);
			qDebug() << "DevciceThread:slept!";
			myPS.BlockToReady(pid);
			qDebug() << "DeviceThread:BlockToReady:" << pid;
			myDeviceSystem.devices[2].isBusy = 0;
			qDebug() << "device[2].isbusy being set to 0";
			deviceToWork = myDeviceSystem.deviceWork();
		}
		sleep(1);
	}
}