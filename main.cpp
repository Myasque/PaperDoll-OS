#include "widget.h"
#include <QApplication>
#include"PROCESS.h"
#include"disk.h"
#include <thread>

#define VIRTUAL_NUMBER 8
#define PHYSICAL_NUMBER 3

using namespace std;

//内存全局变量
memoryManager myMemoryManager;
//文件全局变量
FileSystem myFileSystem;
//设备全局变量
DeviceSystem myDeviceSystem;
//磁盘全局变量
Disk myDisk;
//进程全局变量
PROCESS myPS;

int main(int argc, char* argv[])
{
	myMemoryManager.Initial();
	myDisk.initDisk();
	QApplication a(argc, argv);
	Widget w;
	w.show();
	return a.exec();
}

