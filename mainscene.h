#ifndef MAINSCENE_H
#define MAINSCENE_H

//此界面为主界面，显示所有的服务
#include <QWidget>
#include<QString>
#include <QLCDNumber>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QLabel>
#include <QFile>
#include <QVBoxLayout>
#include"mythread.h"
#include"threadDevice.h"
#include "button.h"
#include "inputwidget.h"
#include "outputwidget.h"
#include"FileSystem.h"
#include "threadDevice.h"

#define BUTTONNUM 18

struct SystemTime
{
	int hour;
	int min;
	int second;
};

class MainScene : public QWidget
{
	Q_OBJECT

private:
	MyThread* thread1;

	ThreadDevice* thread2;

public://函数
	explicit MainScene(QWidget* parent = nullptr);//设置主界面初始布局

	void init();//初始化

	void setPageLayout();//设置窗口和布局

	void updateUI();


public://变量

	/*服务栏*/
//    Button *mybutton[BUTTONNUM];
	Button* proInfo;//进程参数
	Button* proQueue;//进程队列
	Button* synShow;//同步演示
	Button* suspend;//中断

	Button* fileTree;//文件树
	Button* createContent;//创建目录
	Button* modifyContent;//修改目录
	Button* createFile;//创建文件
	Button* runFile;//运行文件
	Button* deleteFile;//删除文件
	Button* readFile;//读取文件
	Button* writeFile;//写入文件

	Button* diskIfo;//监控资源
	Button* memoryUsage;//内存占用情况
	Button* diskUsage;//磁盘占用情况
	Button* deviceInfo;//设备状态
	Button* addDevice;//添加设备

	Button* backBtns;//返回按钮

	QString nowPathString;
	QLabel* nowPath;//当前所在路径

	QLabel* OutputSys;//同步演示

	InputWidget* iw;//显示输入窗口

	OutputWidget* ow;//显示输出窗口

	InputWidget* iw1;//写入文件窗口

	int flag = 0;
	int flag2 = 0;
	int countSys = 0;
	int countMon = 0;
	int interruptPid;
	QString WriteFileName;//zmy

	QtCharts::QChart* chart;
	QtCharts::QChartView* chartView;

	QtCharts::QLineSeries* dataSeries;

	QtCharts::QValueAxis* axisX;
	QtCharts::QValueAxis* axisY;


	/*系统计时器*/
//    QLCDNumber *tm_widget;//显示时间当前时间

//    SystemTime time;//系统时间




public slots:

	void ShowProInfo();//展示进程参数

	void ShowProQueue();//展示进程队列

	void ShowSys();//同步演示

	void InterrputProcess();//中断进程

	void ShowFileTree();//文件树

	void CreateDir();//创建目录

	void ModifyDir();//修改目录

	void CreateFile();//创建文件

	void RunFile();//运行文件

	void DeleteFile();//删除文件

	void ReadFile();//读取文件

	void WriteFile();//写入文件

	void DiskIfo();//监控磁盘

	void MemoryUsage();//内存占用情况

	void DiskUsage();//磁盘占用情况

	void DeviceStatus();//展示设备状态

	void AddDevice();//添加设备

	void openThreadBtnSlot();//zmy

signals:
	void startThread();//zmy
	void backToWidget();

};

#endif // MAINSCENE_H
