#define MAX_FILE 1000
#include "outputwidget.h"
extern PROCESS myPS;
extern FileSystem myFileSystem;
extern memoryManager myMemoryManager;
extern DeviceSystem myDeviceSystem;
extern Disk myDisk;
OutputWidget::OutputWidget(OutputType type)
{

	//switch分支判断窗口类型
	switch (type) {
	case Process_info:
		this->OutputProcessInfo();
		break;

	case Process_Queue:
		this->OutputProcessQueue();
		break;

	case Show_Sys:
		this->OutputSys();
		break;

	case File_Tree:
		this->OutputFileTree();
		break;

	case Read_Files:
		this->OutputReadFile();
		break;

	case Memory_Usage:
		this->OutputMemoryUsage();
		break;

	case Device_Info:
		this->OutputDeviceStatus();
		break;

	case Disk_output:
		this->OutputDisk();
		break;
	}

}

//展示进程参数
void OutputWidget::OutputProcessInfo()
{
	//第一步，设置窗口
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Show Process Queue");

	//第二步，设置输出文本框
	this->ProQueueMessage = new QLabel("展示进程队列", this);
	//设置文本框内容居中显示
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//设置文本框的坐标和尺寸
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//设置文本框的外观，包括字体的大小和颜色、按钮的背景色
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//设置文本框要显示超链接内容
	if (myPS.pidRunning == -1) { ProQueueMessage->setText("No Current Running Process :("); }
	else { ProQueueMessage->setText(QString::fromStdString(myPS.findPCB(myPS.pidRunning)->outPCB())); }
	//与进程交互

	this->show();
}

//展示进程队列
void OutputWidget::OutputProcessQueue()
{
	//第一步，设置窗口
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Show Process Queue");

	//第二步，设置输出文本框
	this->ProQueueMessage = new QLabel("展示进程队列", this);
	//设置文本框内容居中显示
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//设置文本框的坐标和尺寸
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//设置文本框的外观，包括字体的大小和颜色、按钮的背景色
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//设置文本框要显示超链接内容
	ProQueueMessage->setText(QString::fromStdString(myPS.showPCBQueue()));//与内存交互

	this->show();
}

void OutputWidget::OutputSys()
{
	//第一步，设置窗口
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Synchronous");

	//第二步，设置输出文本框
	this->ProQueueMessage = new QLabel("Show Synchronous", this);
	//设置文本框内容居中显示
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//设置文本框的坐标和尺寸
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//设置文本框的外观，包括字体的大小和颜色、按钮的背景色
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//设置文本框要显示超链接内容
	qDebug() << "heeeeeere";
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [=]() {
		ProQueueMessage->setText(QString::fromStdString(myPS.showPCBQueue()) + QString::number(temp));
		//ProQueueMessage->setText("bobo\nbobo" + QString::number(temp));
		//测试一下每1s内容是否发生变化
		//bobo:QString = 重新获取
		temp = temp + 1;
		qDebug() << temp;
		if (temp >= 8)
		{
			//timer->stop();
			//this->close();
		}
		update();
		});

	timer->start(1000);
	this->show();
}

//文件树
void OutputWidget::OutputFileTree()
{
	//第一步，设置窗口
	//QTreeWidget TreeWidget;
	this->TreeWidget = new QTreeWidget(this);
	TreeWidget->setFixedSize(1200, 800);
	this->setWindowTitle("File Tree");
	TreeWidget->setHeaderHidden(true);

	int treeParent[MAX_FILE];//int数组，用双亲表示法存储树
	memset(treeParent, -1, MAX_FILE);//zmy0602:原来c++要这样才能初始化全部数组TAT
	QStringList fileList;//文件名称string
	QStringList a[MAX_FILE];

	QString FileTree = QString::fromStdString(myFileSystem.printDirectory());
	FileTree.prepend("~$");
	QStringList FileTreeList = FileTree.split("$");
	int num = FileTreeList.size() - 1;//num是文件总数
	for (int i = 0; i < num; i++) { qDebug() << FileTreeList.at(i); }

	QStringListIterator strIterator1(FileTreeList);
	//第一次遍历，得到fileList
	while (strIterator1.hasNext()) {//遍历每一行，得到fileList
		QString tmpQ = strIterator1.next();
		qDebug() << tmpQ << endl;
		QStringList tmpList = tmpQ.split("/");
		fileList << tmpList.at(tmpList.size() - 1);//将一行中最后一个元素，即文件名，插入到FileList中
	}

	//第二次遍历，得到双亲表示法存储数组int treeParent[MAX_FILE]
	QStringListIterator strIterator2(FileTreeList);
	int i = 0;
	while (strIterator2.hasNext()) {//遍历每一行，得到fileList
		QString tmpQ = strIterator2.next();
		qDebug() << tmpQ << endl;
		QStringList tmpList = tmpQ.split("/");
		if (tmpList.size() >= 2) {
			QString tmpQ2 = tmpList.at(tmpList.size() - 2);//得到倒数第二个文件
			int index;
			index = fileList.indexOf(tmpQ2);//得到index
			treeParent[i] = index;
		}
		i++;
	}

	//添加顶层结点
	this->childItem[0] = new QTreeWidgetItem(TreeWidget);
	childItem[0]->setText(0, "myDir");
	TreeWidget->addTopLevelItem(childItem[0]);
	//第三次遍历，给每个文件创造item，并按照树建立父子关系
	for (int i = 1; i < num; i++) {
		a[i] << fileList.at(i);
		if (treeParent[i] != -1) {
			int tmp = treeParent[i];
			this->childItem[i] = new QTreeWidgetItem(childItem[tmp], a[i]);
			qDebug() << "i=" << i;
		}

	}

	this->show();
}

//读文件
void OutputWidget::OutputReadFile()
{
	//第一步，设置窗口
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Read File");

	//第二步，设置输出文本框
	this->ProQueueMessage = new QLabel("读文件", this);
	//设置文本框内容居中显示
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//设置文本框的坐标和尺寸
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//设置文本框的外观，包括字体的大小和颜色、按钮的背景色
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//设置文本框要显示超链接内容
	ProQueueMessage->setText(QString::fromStdString((myPS.findPCB(myPS.pidMax - 1))->bufferIO));
	qDebug() << "output widget text is:" << QString::fromStdString((myPS.findPCB(myPS.pidMax - 1))->bufferIO);
	this->show();
}

void OutputWidget::OutputMemoryUsage()
{
	//第一步，设置窗口
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Memory Usage");

	//第二步，设置输出文本框
	this->ProQueueMessage = new QLabel("Memory Usage", this);
	//设置文本框内容居中显示
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//设置文本框的坐标和尺寸
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//设置文本框的外观，包括字体的大小和颜色、按钮的背景色
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//设置文本框要显示超链接内容
	ProQueueMessage->setText(QString::fromStdString(myMemoryManager.watchMemory()));//与内存交互

	this->show();
}

void OutputWidget::OutputDeviceStatus()
{
	//第一步，设置窗口
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Device Status");

	//第二步，设置输出文本框
	this->ProQueueMessage = new QLabel("Device Status", this);
	//设置文本框内容居中显示
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//设置文本框的坐标和尺寸
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//设置文本框的外观，包括字体的大小和颜色、按钮的背景色
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//设置文本框要显示超链接内容
	ProQueueMessage->setText(QString::fromStdString(myDeviceSystem.showDevices()));//与设备交互

	this->show();
}

void OutputWidget::OutputDisk()
{
	//第一步，设置窗口
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Disk Infomation");
	//第二步，设置输出文本框
	this->ProQueueMessage = new QLabel("Disk Infomation", this);
	//设置文本框内容居中显示
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//设置文本框的坐标和尺寸
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//设置文本框的外观，包括字体的大小和颜色、按钮的背景色
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//设置文本框要显示超链接内容
	ProQueueMessage->setText(QString::fromStdString(myDisk.showDisk()));//与disk交互
	qDebug() << QString::fromStdString(myDisk.showDisk());
	this->show();
}
