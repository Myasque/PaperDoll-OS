#include "widget.h"
extern int isMLFQ;  //�Ƿ�ѡ��MLFQ�㷨����=1����=0
extern int isSJF;  //�Ƿ�ѡ��SJF�㷨����=1����=0
extern int isFCFS;  //�Ƿ�ѡ��FCFS�㷨����=1����=0
extern int isPreemptive; //�Ƿ���ռ����=1����=0����SJFģʽ��ʹ��

extern int diskAlgorithm;//���̵�Ѱ���㷨

extern int replaceStrategy;//�ڴ�Ļ�ҳ�㷨

Widget::Widget(QWidget* parent)
	: QWidget(parent)
{

	this->on_Item_clicked();

	//    int x = 10;
		//���ô���
	this->setFixedSize(640, 360);
	this->setWindowTitle("Paper Doll Operation System");

	//��ʾ��ڽ��� 1.���� 2.�˳�
	//��������button��һ��QLabel

	//��ǩ
	QFont font;
	font.setFamily("΢���ź�");
	font.setPointSize(20);
	this->title = new QLabel(this);
	this->title->setText("Paper Doll OS");
	this->title->setFixedSize(320, 180);
	this->setFont(font);
	this->title->move(this->width() * 0.5 - this->title->width() * 0.5, 30);

	//����
	this->enter = new Button(this, "Enter");
	this->enter->move(this->width() * 0.5 - this->enter->width() * 0.5, 180);
	this->enter->setStyleSheet("Button{background-color:#FFB5C5;};");

	//�˳�
	this->exit = new Button(this, "Edit");
	this->exit->move(this->width() * 0.5 - this->exit->width() * 0.5, 280);
	this->exit->setStyleSheet("Button{background-color:#CDC1C5;};");

	//���ò��֣���ѡ��

	//����connect
	//1��������룬���ص�ǰ���棬��ʾ������mainscene

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

	//2������˳����رյ�ǰ����
	connect(this->exit, &QPushButton::clicked, this, [&]() {

		this->close();
		});
}

void Widget::on_Item_clicked()
{
	//����ѡ��Ի���ѡ������㷨
	//ѡ����̵����㷨
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

	//ѡ���ڴ滻ҳ��ʽ
	QStringList MemoryStorageList;
	MemoryStorageList << "FIFO" << "LRU";

	this->MemoryStorageitem = QInputDialog::getItem(this,
		"Memory page in/out Algorithms",
		"Memory page in/out Algorithms:       ",
		MemoryStorageList);
	if (MemoryStorageitem == "FIFO") { replaceStrategy = 0; }
	if (MemoryStorageitem == "LRU") { replaceStrategy = 1; }

	//ѡ���ͷѰ���㷨
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

