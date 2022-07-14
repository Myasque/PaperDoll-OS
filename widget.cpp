#include "widget.h"
extern int isMLFQ;  //是否选择MLFQ算法，是=1，否=0
extern int isSJF;  //是否选择SJF算法，是=1，否=0
extern int isFCFS;  //是否选择FCFS算法，是=1，否=0
extern int isPreemptive; //是否抢占，是=1，否=0，仅SJF模式下使用

extern int diskAlgorithm;//磁盘的寻道算法

extern int replaceStrategy;//内存的换页算法

Widget::Widget(QWidget* parent)
	: QWidget(parent)
{

	this->on_Item_clicked();

	//    int x = 10;
		//设置窗口
	this->setFixedSize(640, 360);
	this->setWindowTitle("Paper Doll Operation System");

	//显示入口界面 1.进入 2.退出
	//定义两个button和一个QLabel

	//标签
	QFont font;
	font.setFamily("微软雅黑");
	font.setPointSize(20);
	this->title = new QLabel(this);
	this->title->setText("Paper Doll OS");
	this->title->setFixedSize(320, 180);
	this->setFont(font);
	this->title->move(this->width() * 0.5 - this->title->width() * 0.5, 30);

	//进入
	this->enter = new Button(this, "Enter");
	this->enter->move(this->width() * 0.5 - this->enter->width() * 0.5, 180);
	this->enter->setStyleSheet("Button{background-color:#FFB5C5;};");

	//退出
	this->exit = new Button(this, "Edit");
	this->exit->move(this->width() * 0.5 - this->exit->width() * 0.5, 280);
	this->exit->setStyleSheet("Button{background-color:#CDC1C5;};");

	//设置布局（可选）

	//两个connect
	//1、点击进入，隐藏当前界面，显示主界面mainscene

	connect(this->enter, &QPushButton::clicked, this, [&]() {

		this->hide();

		this->scene = new MainScene;
		//this->scene->setGeometry(this->geometry());
		this->scene->show();

		connect(scene, &MainScene::backToWidget, this, [&]() {

			//  this->setGeometry(scene->geometry());
			scene->close();
			//            delete scene;
			//            scene = NULL;

			this->show();
			});

		});

	//2、点击退出，关闭当前界面
	connect(this->exit, &QPushButton::clicked, this, [&]() {

		this->close();
		});
}

void Widget::on_Item_clicked()
{
	//先在选择对话框选择各种算法
	//选择进程调度算法
	QStringList ProcessSchAlgList;
	ProcessSchAlgList << "FCFS" << "SJF" << "SRTF" << "Multi-level Feedback Queue";

	this->ProcessSchAlgitem = QInputDialog::getItem(this,
		"Process Scheduling Algorithms",
		"Process Scheduling Algorithms:   ",
		ProcessSchAlgList);
	if (ProcessSchAlgitem == "FCFS") { isFCFS = 1; }
	else if (ProcessSchAlgitem == "SJF") { isSJF = 1; }
	else if (ProcessSchAlgitem == "SRTF") { isSJF = 1; isPreemptive = 1; }
	else isMLFQ = 1;

	//选择内存换页方式
	QStringList MemoryStorageList;
	MemoryStorageList << "FIFO" << "LRU";

	this->MemoryStorageitem = QInputDialog::getItem(this,
		"Memory page in/out Algorithms",
		"Memory page in/out Algorithms:       ",
		MemoryStorageList);
	if (MemoryStorageitem == "FIFO") { replaceStrategy = 0; }
	if (MemoryStorageitem == "LRU") { replaceStrategy = 1; }

	//选择磁头寻道算法
	QStringList HeadSeekAlgList;
	HeadSeekAlgList << "FCFS" << "SSTF" << "SCAN" << "C-SCAN" << "LOOK" << "C-LOOK";

	this->HeadSeekAlgitem = QInputDialog::getItem(this,
		"Head Seek Algorithms",
		"Head Seek Algorithms: ",
		HeadSeekAlgList);
	if (HeadSeekAlgitem == "FCFS") { diskAlgorithm = 1; }
	if (HeadSeekAlgitem == "SSTF") { diskAlgorithm = 2; }
	if (HeadSeekAlgitem == "SCAN") { diskAlgorithm = 3; }
	if (HeadSeekAlgitem == "C-SCAN") { diskAlgorithm = 4; }
	if (HeadSeekAlgitem == "LOOK") { diskAlgorithm = 5; }
	if (HeadSeekAlgitem == "C-LOOK") { diskAlgorithm = 6; }

}

Widget::~Widget()
{

}

