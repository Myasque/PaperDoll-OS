#include "mainscene.h"
#include"disk.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QMessageBox>
#include <QtDebug>

extern FileSystem myFileSystem;
extern PROCESS myPS;
extern Disk myDisk;
extern DeviceSystem myDeviceSystem;
extern int diskAlgorithm;

MainScene::MainScene(QWidget* parent) : QWidget(parent)
{
	/*第一步，加载文件，初始化信息等*/

	//初始化信息
	this->init();

	/*线程初始化*/ //zmy
	thread1 = new MyThread();
	thread2 = new ThreadDevice();
	//thread1->start();//zmy:open it!

	/*第二步，设置窗口整体布局*/
	//设置窗口和布局
	this->setPageLayout();

	/*第三步，将每个按钮连接到对应的函数*/

	//1.连接到函数

	connect(this->proInfo, &QPushButton::clicked, this, &MainScene::ShowProInfo);
	connect(this->proQueue, &QPushButton::clicked, this, &MainScene::ShowProQueue);
	connect(this->synShow, &QPushButton::clicked, this, &MainScene::ShowSys);
	connect(this->suspend, &QPushButton::clicked, this, &MainScene::InterrputProcess);//zmy

	connect(this->synShow, &QPushButton::clicked, this, &MainScene::openThreadBtnSlot);//zmy

	connect(this->fileTree, &QPushButton::clicked, this, &MainScene::ShowFileTree);
	connect(this->createContent, &QPushButton::clicked, this, &MainScene::CreateDir);
	connect(this->modifyContent, &QPushButton::clicked, this, &MainScene::ModifyDir);
	connect(this->createFile, &QPushButton::clicked, this, &MainScene::CreateFile);
	connect(this->runFile, &QPushButton::clicked, this, &MainScene::RunFile);
	connect(this->deleteFile, &QPushButton::clicked, this, &MainScene::DeleteFile);
	connect(this->readFile, &QPushButton::clicked, this, &MainScene::ReadFile);
	connect(this->writeFile, &QPushButton::clicked, this, &MainScene::WriteFile);

	connect(this->diskIfo, &QPushButton::clicked, this, &MainScene::DiskIfo);//zmy

	connect(this->memoryUsage, &QPushButton::clicked, this, &MainScene::MemoryUsage);
	connect(this->diskUsage, &QPushButton::clicked, this, &MainScene::DiskUsage);
	connect(this->addDevice, &QPushButton::clicked, this, &MainScene::AddDevice);
	connect(this->deviceInfo, &QPushButton::clicked, this, &MainScene::DeviceStatus);

	//2.关闭窗口，打开开始界面
	connect(this->backBtns, &QPushButton::clicked, this, [=]() {

		emit this->backToWidget();

		});


}


void MainScene::openThreadBtnSlot()
{
	/*开启一个线程*/
	thread1->start();
	thread2->start();
	//thread1->isRunning();
	qDebug() << "Main Thread id : " << QThread::currentThreadId();
	qDebug() << "thread1 is running:" << QString::number(thread1->isRunning());
	qDebug() << "thread2 is running:" << QString::number(thread2->isRunning());
}

void MainScene::init()
{
	nowPathString = "";
}


void MainScene::setPageLayout()
{
	//设置窗口
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Paper Doll OS Service");
	this->setWindowTitle("Paper Doll OS Service");

	//定义button
	this->proInfo = new Button(this, "Process Info");
	this->proQueue = new Button(this, "Process Queues");
	this->synShow = new Button(this, "Synchronous");
	this->suspend = new Button(this, "Interrupt");
	this->backBtns = new Button(this, "Return");

	this->fileTree = new Button(this, "File Tree");
	this->createContent = new Button(this, "Create Dir");
	this->createFile = new Button(this, "Create File");
	this->modifyContent = new Button(this, "Change Route");
	this->runFile = new Button(this, "Run File");
	this->deleteFile = new Button(this, "Del File");
	this->readFile = new Button(this, "Read File");
	this->writeFile = new Button(this, "Write File");

	this->diskIfo = new Button(this, "Disk Ifo");
	this->memoryUsage = new Button(this, "Mon Memery");
	this->diskUsage = new Button(this, "Mon Disk");
	this->deviceInfo = new Button(this, "Device Info");
	this->addDevice = new Button(this, "Add Device");



	//设置按钮的位置
	int x = this->width() * 0.1;
	int y = this->height() * 0.1;
	int spaceX = proInfo->width() * 1.2;
	int spaceY = proInfo->height() * 1.2;
	proInfo->move(x, y);
	proQueue->move(x + spaceX, y);
	synShow->move(x + 2 * spaceX, y);
	suspend->move(x + 3 * spaceX, y);

	//当前所在路径
	QString realPath = QString::fromStdString(myFileSystem.printRoute());
	this->nowPath = new QLabel(this);
	this->nowPath->setText("Current Path:" + realPath);
	this->setFont(QFont("微软雅黑", 10));
	this->nowPath->move(x, y + 1.5 * spaceY);

	fileTree->move(x, y + 2 * spaceY);
	createContent->move(x + spaceX, y + 2 * spaceY);
	modifyContent->move(x + 2 * spaceX, y + 2 * spaceY);
	createFile->move(x + 3 * spaceX, y + 2 * spaceY);
	runFile->move(x, y + 3 * spaceY);
	deleteFile->move(x + spaceX, y + 3 * spaceY);
	readFile->move(x + 2 * spaceX, y + 3 * spaceY);
	writeFile->move(x + 3 * spaceX, y + 3 * spaceY);
	//    qDebug() << 3*spaceX+writeFile->width();

	x = this->width() * 0.78 + this->width() * 0.1 - diskIfo->width() * 0.5;
	y = this->height() * 0.20;
	int space = this->height() * 0.05 + diskIfo->height();
	diskIfo->move(x, y);
	memoryUsage->move(x, y + space);
	diskUsage->move(x, y + 2 * space);
	deviceInfo->move(x, y + 3 * space);
	addDevice->move(x, y + 4 * space);

	backBtns->move(this->width() - backBtns->width() * 1.2, this->height() - backBtns->height() * 1.2);
	backBtns->setStyleSheet("Button{background-color:#CDC1C5;};");
	//    qDebug() << this->height()-backBtns->height()*1.2;



	//    //设置定时器的位置
	//    tm_widget = new QLCDNumber(this);

	//    //设置位置
	//    tm_widget->resize(this->width()*0.16,50);
	//    tm_widget->move(this->width()*0.80,this->height()*0.03);

	//    //设置样式和模式
	//    tm_widget->setMode(QLCDNumber::Dec);
	//    tm_widget->setDigitCount(10);
	//    tm_widget->setSegmentStyle(QLCDNumber::Flat);
	//    tm_widget->setStyleSheet("border: 2px solid black; color: black; background: white;");

}

void MainScene::ShowProInfo()
{
	//打开OutputWidget
	ow = new OutputWidget(Process_info);
	//ow->setAttribute(Qt::WA_DeleteOnClose);
	//ow->setModal(true);
}

void MainScene::ShowProQueue()
{
	ow = new OutputWidget(Process_Queue);
	//ow->setAttribute(Qt::WA_DeleteOnClose);
	//ow->setModal(true);
}

void MainScene::InterrputProcess() {
	//int tmpPid = 0;

	if (flag2 == 0) {
		//myPS.
		interruptPid = myPS.pidRunning;
		myPS.RunningToBlock(interruptPid);
		flag2 = 1;
	}
	else {
		qDebug() << "interruptPid=" << interruptPid;
		myPS.BlockToReady(interruptPid);
		flag2 = 0;
	}
}
/*
void MainScene::ShowSys()
{
	ow = new OutputWidget(Show_Sys);
	ow->setAttribute(Qt::WA_DeleteOnClose);
	ow->setModal(true);
}
*/
void MainScene::ShowFileTree()
{
	ow = new OutputWidget(File_Tree);
	//ow->setAttribute(Qt::WA_DeleteOnClose);
	//ow->setModal(true);
}

void MainScene::CreateDir()
{
	//打开InputWidget
	iw = new InputWidget(Create_Dir);
	//iw->setAttribute(Qt::WA_DeleteOnClose);
	//iw->setModal(true);
	iw->show();
	connect(iw, &InputWidget::accept, this, [&]() {
		QString NewDirName = iw->GetNewDir();

		iw->close();

		});
}

void MainScene::ModifyDir()
{
	//this->hide();
	iw = new InputWidget(Modify_Dir);
	//iw->setAttribute(Qt::WA_DeleteOnClose);
	//iw->setModal(true);
	iw->show();

	//修改界面上的当前路径
	connect(iw, &InputWidget::accept, this, [&]() {
		nowPathString = iw->GetNewPath();
		this->nowPath->clear();
		QString realPath = QString::fromStdString(myFileSystem.printRoute());//realpath正确
		this->nowPath->setText("Current Path:" + realPath);
		this->nowPath->adjustSize();

		//释放空间
		iw->close();

		//this->show();
		});
}

void MainScene::CreateFile()
{
	//this->hide();
	iw = new InputWidget(Create_File);
	//iw->setAttribute(Qt::WA_DeleteOnClose);
	//iw->setModal(true);
	iw->show();

	connect(iw, &InputWidget::accept, this, [&]() {
		QString NewFileName = iw->GetNewFile();

		//释放空间
		iw->close();

		//this->show();
		});
}

void MainScene::RunFile()
{
	//this->hide();
	iw = new InputWidget(Run_File);
	//iw->setAttribute(Qt::WA_DeleteOnClose);
	//iw->setModal(true);
	iw->show();

	connect(iw, &InputWidget::accept, this, [&]() {
		QString RunFileName = iw->GetNewFile();
		QString Priority = iw->NewFileMap[RunFileName];
		string fileName = RunFileName.toStdString();
		int Pri = Priority.toInt();

		//传给其他模块要运行的文件名

		int returnNum = myPS.createProcess(Pri, fileName);//执行读文件的进程//交互部分
		if (returnNum == -1)//如果创建内存失败
		{
			QMessageBox warn;
			warn.setWindowTitle("Warning");
			warn.setIcon(QMessageBox::Warning);
			warn.setStandardButtons(QMessageBox::Ok);
			warn.setButtonText(QMessageBox::Ok, QString("Accept"));
			warn.setText("Memory isnot Availabel!Failed to Create Process");
			warn.exec();
		}
		else {
			myFileSystem.readFile(fileName, myPS.pidMax - 1);
			myPS.findPCB(myPS.pidMax - 1)->timeLast = myFileSystem.IRnum * 2;//时间赋值
			if (isSJF && isPreemptive && myPS.pidRunning != -1)  //如果CPU调度算法为抢占式SJF
				myPS.preeSJF();  //每当有进程进入就绪队列时，检查是否需要抢占
		}

		//释放空间
		iw->close();

		//this->show();
		});
}

void MainScene::DeleteFile()
{
	//this->hide();
	iw = new InputWidget(Del_File);
	//iw->setAttribute(Qt::WA_DeleteOnClose);
	//iw->setModal(true);
	iw->show();

	connect(iw, &InputWidget::accept, this, [&]() {
		QString DeleteFileName = iw->GetNewFile();
		//传给其他模块文件名

		//释放空间
		iw->close();

		//this->show();
		});
}

void MainScene::ReadFile()
{
	//this->hide();
	iw = new InputWidget(Read_File);
	iw->setAttribute(Qt::WA_DeleteOnClose);
	iw->setModal(true);
	iw->show();

	connect(iw, &InputWidget::accept, this, [&]() {
		QString ReadFileName = iw->GetNewFile();
		//传给其他模块文件名
		string fileName = ReadFileName.toStdString();
		int returnNum = myPS.createProcess(0, fileName);//执行读文件的进程//交互部分
		if (returnNum == -1)//如果创建内存失败
		{
			QMessageBox warn;
			warn.setWindowTitle("Warning");
			warn.setIcon(QMessageBox::Warning);
			warn.setStandardButtons(QMessageBox::Ok);
			warn.setButtonText(QMessageBox::Ok, QString("Accept"));
			warn.setText("Memory isnot Availabel!Failed to Create Process");
			warn.exec();
		}
		else
		{
			//qDebug() << "createProcess,filename= " << ReadFileName;
			myFileSystem.readFile(".readFile.e", myPS.pidMax - 1);
			//myPS.runProcess(myPS.pidMax - 1);//zmy:加上schedua线程后由其完成调度执行
			//myPS.scheduling();//zmy:发现还是要加一个schedual
			//myPS.ReadyToRunning(myPS.pidMax - 1);
			myPS.runProcessAll(myPS.pidMax - 1);//zmy:新想法

			iw->close();
			//this->show();
			ow = new OutputWidget(Read_Files);
			ow->setModal(true);
		}
		});
}

void MainScene::WriteFile()
{
	//this->hide();
	iw = new InputWidget(Write_File);
	iw->setAttribute(Qt::WA_DeleteOnClose);
	iw1 = new InputWidget(Write_Something);
	iw1->setAttribute(Qt::WA_DeleteOnClose);
	iw->setModal(true);

	iw->show();
	connect(iw, &InputWidget::accept, this, [&]() {

		WriteFileName = iw->GetNewFile();
		//传给其他模块要运行的文件名
		iw->close();
		iw1->setModal(true);
		iw1->show();

		connect(iw1, &InputWidget::reaccept, this, [&]() {
			QString writeSometing = iw1->GetWriteSomething();

			qDebug() << "fileName is" << WriteFileName;
			qDebug() << "writeSomething is:" << writeSometing;
			//把用户输入内容写入进程buffer
			int returnNum = myPS.createProcess(0, WriteFileName.toStdString());
			if (returnNum == -1)//如果创建内存失败
			{
				QMessageBox warn;
				warn.setWindowTitle("Warning");
				warn.setIcon(QMessageBox::Warning);
				warn.setStandardButtons(QMessageBox::Ok);
				warn.setButtonText(QMessageBox::Ok, QString("Accept"));
				warn.setText("Memory isnot Availabel!Failed to Create Process");
				warn.exec();
			}
			else {
				myPS.findPCB(myPS.pidMax - 1)->bufferIO = writeSometing.toStdString();
				myFileSystem.readFile(".writeFile.e", myPS.pidMax - 1);
				myPS.ReadyToRunning(myPS.pidMax - 1);
				myPS.runProcessAll(myPS.pidMax - 1);
			}

			iw1->close();

			});

		});
}

void MainScene::MemoryUsage()
{
	ow = new OutputWidget(Memory_Usage);
	//ow->setAttribute(Qt::WA_DeleteOnClose);
	//ow->setModal(true);
}

void MainScene::DiskIfo() {
	ow = new OutputWidget(Disk_output);
}

void MainScene::DiskUsage()
{
	QTimer* timer = new QTimer(this);

	if (flag == 0)
	{
		chart = new QtCharts::QChart();
		dataSeries = new QtCharts::QLineSeries;//直线
		switch (diskAlgorithm)
		{
		case 1:
			dataSeries->setName("FCFS");
			break;
		case 2:
			dataSeries->setName("SSTF");
			break;
		case 3:
			dataSeries->setName("SCAN");
			break;
		case 4:
			dataSeries->setName("C_SCA");
			break;
		case 5:
			dataSeries->setName("LOOK");
			break;
		case 6:
			dataSeries->setName("C_LOOK");
			break;
		}
		//zmy:与磁盘部分交互
		myDisk.seekPath(diskAlgorithm);//寻道

		//qDebug() << myDisk.output[0];
		//qDebug() << myDisk.output[1];

		axisX = new QtCharts::QValueAxis(chart);
		axisX->setRange(0, 200);
		axisX->setTitleText("track number");
		axisX->setGridLineVisible(true);//是否显示网格线


		axisY = new QtCharts::QValueAxis(chart);
		axisY->setRange(0, myDisk.outputNum);//boboy轴从0到磁盘使用次数*2，记得把头文件里面的count初始化一致！！
		axisY->setTitleText("Count");
		axisY->setGridLineVisible(false);//是否显示网格线

		//注意在使用setAxisX/Y之前，QSeries必须要加载至chart否则图上没有数据
		chart->addSeries(dataSeries);
		chart->setAxisX(axisX, dataSeries);
		chart->setAxisY(axisY, dataSeries);
		//设置系统主题ChartThemeBlueCerulean、ChartThemeBlueIcy\ChartThemeBrownSand\ChartThemeQt
		chart->setTheme(QtCharts::QChart::ChartThemeBrownSand);
		this->chartView = new QtCharts::QChartView(this);

		chartView->setChart(chart);
		chartView->move(120, 320);
		chartView->resize(690, 420);
		chartView->show();

		countMon = myDisk.outputNum;
		connect(timer, &QTimer::timeout, this, [=]() {
			//每隔1s画一条线
			//bobo每次加一个点
			if (countMon == myDisk.outputNum) {
				dataSeries->append(QPoint(myDisk.curHead, countMon)); qDebug() << "curHead=" << myDisk.curHead << " contMon=" << countMon;

			}
			else { dataSeries->append(QPoint(myDisk.output[(myDisk.outputNum - 1) - countMon], countMon)); }
			countMon -= 1;

			if (countMon == 0)
			{
				//再次点击该按钮取消画面
				flag = 1;
			}
			update();
			});
		timer->start(1000);
	}
	if (flag == 1)
	{
		chartView->close();
		flag = 0;
		timer->stop();
	}

}

void MainScene::DeviceStatus()
{
	ow = new OutputWidget(Device_Info);
	//ow->setAttribute(Qt::WA_DeleteOnClose);
	//ow->setModal(true);
}

void MainScene::AddDevice()
{
	this->hide();
	iw = new InputWidget(Add_Device);
	iw->setAttribute(Qt::WA_DeleteOnClose);
	iw->setModal(true);
	iw->show();

	connect(iw, &InputWidget::accept, this, [&]() {
		QString DeviceName = iw->GetNewFile();
		//传给其他模块设备名
		qDebug() << DeviceName;
		myDeviceSystem.createDevice(DeviceName.toStdString());
		iw->close();

		this->show();
		});
}

void MainScene::ShowSys() {
	this->OutputSys = new QLabel("Show Sys", this);
	//设置文本框内容居中显示
	OutputSys->setAlignment(Qt::AlignLeft);
	//设置文本框的坐标和尺寸
	OutputSys->setGeometry(120, 320, 690, 420);
	//设置文本框的外观，包括字体的大小和颜色、按钮的背景色
	OutputSys->setStyleSheet("QLabel{font:20px;color:#B39B72;background-color:#F3ECE0;}");
	//设置文本框要显示超链接内容

	countSys = 0;
	//qDebug() << "countSys=" << countSys;
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [=]() {
		OutputSys->setText(QString::fromStdString(myPS.showPCBQueue()) + QString::number(countSys));
		qDebug() << "countSys=" << countSys;
		countSys += 1;
		if (countSys >= 100)
		{
			countSys = 0;
			timer->stop();
			//OutputSys->close();
		}
		update();
		});

	timer->start(1000);
	OutputSys->show();
}