#define MAX_FILE 1000
#include "outputwidget.h"
extern PROCESS myPS;
extern FileSystem myFileSystem;
extern memoryManager myMemoryManager;
extern DeviceSystem myDeviceSystem;
extern Disk myDisk;

OutputWidget::OutputWidget(OutputType type)
{

	//switch��֧�жϴ�������
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

//չʾ���̲���
void OutputWidget::OutputProcessInfo()
{
	//��һ�������ô���
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Show Process Queue");

	//�ڶ�������������ı���
	this->ProQueueMessage = new QLabel("չʾ���̶���", this);
	//�����ı������ݾ�����ʾ
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//�����ı��������ͳߴ�
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//�����ı������ۣ���������Ĵ�С����ɫ����ť�ı���ɫ
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//�����ı���Ҫ��ʾ����������
	if (myPS.pidRunning == -1) { ProQueueMessage->setText("No Current Running Process :("); }
	else { ProQueueMessage->setText(QString::fromStdString(myPS.findPCB(myPS.pidRunning)->outPCB())); }
	//����̽���

	this->show();
}

//չʾ���̶���
void OutputWidget::OutputProcessQueue()
{
	//��һ�������ô���
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Show Process Queue");

	//�ڶ�������������ı���
	this->ProQueueMessage = new QLabel("չʾ���̶���", this);
	//�����ı������ݾ�����ʾ
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//�����ı��������ͳߴ�
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//�����ı������ۣ���������Ĵ�С����ɫ����ť�ı���ɫ
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//�����ı���Ҫ��ʾ����������
	ProQueueMessage->setText(QString::fromStdString(myPS.showPCBQueue()));//���ڴ潻��

	this->show();
}

void OutputWidget::OutputSys()
{
	//��һ�������ô���
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Synchronous");

	//�ڶ�������������ı���
	this->ProQueueMessage = new QLabel("Show Synchronous", this);
	//�����ı������ݾ�����ʾ
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//�����ı��������ͳߴ�
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//�����ı������ۣ���������Ĵ�С����ɫ����ť�ı���ɫ
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//�����ı���Ҫ��ʾ����������
	qDebug() << "heeeeeere";
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [=]() {
		ProQueueMessage->setText(QString::fromStdString(myPS.showPCBQueue()) + QString::number(temp));
		//ProQueueMessage->setText("bobo\nbobo" + QString::number(temp));
		//����һ��ÿ1s�����Ƿ����仯
		//bobo:QString = ���»�ȡ
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

//�ļ���
void OutputWidget::OutputFileTree()
{
	//��һ�������ô���
	//QTreeWidget TreeWidget;
	this->TreeWidget = new QTreeWidget(this);
	TreeWidget->setFixedSize(1200, 800);
	this->setWindowTitle("File Tree");
	TreeWidget->setHeaderHidden(true);

	int treeParent[MAX_FILE];//int���飬��˫�ױ�ʾ���洢��
	memset(treeParent, -1, MAX_FILE);//zmy0602:ԭ��c++Ҫ�������ܳ�ʼ��ȫ������TAT
	QStringList fileList;//�ļ�����string
	QStringList a[MAX_FILE];

	QString FileTree = QString::fromStdString(myFileSystem.printDirectory());
	FileTree.prepend("~$");
	QStringList FileTreeList = FileTree.split("$");
	int num = FileTreeList.size() - 1;//num���ļ�����
	for (int i = 0; i < num; i++) { qDebug() << FileTreeList.at(i); }

	QStringListIterator strIterator1(FileTreeList);
	//��һ�α������õ�fileList
	while (strIterator1.hasNext()) {//����ÿһ�У��õ�fileList
		QString tmpQ = strIterator1.next();
		qDebug() << tmpQ << endl;
		QStringList tmpList = tmpQ.split("/");
		fileList << tmpList.at(tmpList.size() - 1);//��һ�������һ��Ԫ�أ����ļ��������뵽FileList��
	}

	//�ڶ��α������õ�˫�ױ�ʾ���洢����int treeParent[MAX_FILE]
	QStringListIterator strIterator2(FileTreeList);
	int i = 0;
	while (strIterator2.hasNext()) {//����ÿһ�У��õ�fileList
		QString tmpQ = strIterator2.next();
		qDebug() << tmpQ << endl;
		QStringList tmpList = tmpQ.split("/");
		if (tmpList.size() >= 2) {
			QString tmpQ2 = tmpList.at(tmpList.size() - 2);//�õ������ڶ����ļ�
			int index;
			index = fileList.indexOf(tmpQ2);//�õ�index
			treeParent[i] = index;
		}
		i++;
	}

	//��Ӷ�����
	this->childItem[0] = new QTreeWidgetItem(TreeWidget);
	childItem[0]->setText(0, "myDir");
	TreeWidget->addTopLevelItem(childItem[0]);
	//�����α�������ÿ���ļ�����item�����������������ӹ�ϵ
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

//���ļ�
void OutputWidget::OutputReadFile()
{
	//��һ�������ô���
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Read File");

	//�ڶ�������������ı���
	this->ProQueueMessage = new QLabel("���ļ�", this);
	//�����ı������ݾ�����ʾ
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//�����ı��������ͳߴ�
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//�����ı������ۣ���������Ĵ�С����ɫ����ť�ı���ɫ
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//�����ı���Ҫ��ʾ����������
	ProQueueMessage->setText(QString::fromStdString((myPS.findPCB(myPS.pidMax - 1))->bufferIO));
	qDebug() << "output widget text is:" << QString::fromStdString((myPS.findPCB(myPS.pidMax - 1))->bufferIO);
	this->show();
}

void OutputWidget::OutputMemoryUsage()
{
	//��һ�������ô���
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Memory Usage");

	//�ڶ�������������ı���
	this->ProQueueMessage = new QLabel("Memory Usage", this);
	//�����ı������ݾ�����ʾ
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//�����ı��������ͳߴ�
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//�����ı������ۣ���������Ĵ�С����ɫ����ť�ı���ɫ
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//�����ı���Ҫ��ʾ����������
	ProQueueMessage->setText(QString::fromStdString(myMemoryManager.watchMemory()));//���ڴ潻��

	this->show();
}

void OutputWidget::OutputDeviceStatus()
{
	//��һ�������ô���
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Device Status");

	//�ڶ�������������ı���
	this->ProQueueMessage = new QLabel("Device Status", this);
	//�����ı������ݾ�����ʾ
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//�����ı��������ͳߴ�
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//�����ı������ۣ���������Ĵ�С����ɫ����ť�ı���ɫ
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//�����ı���Ҫ��ʾ����������
	ProQueueMessage->setText(QString::fromStdString(myDeviceSystem.showDevices()));//���豸����

	this->show();
}

void OutputWidget::OutputDisk()
{
	//��һ�������ô���
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Disk Infomation");
	//�ڶ�������������ı���
	this->ProQueueMessage = new QLabel("Disk Infomation", this);
	//�����ı������ݾ�����ʾ
	ProQueueMessage->setAlignment(Qt::AlignLeft);
	//�����ı��������ͳߴ�
	ProQueueMessage->setGeometry(0, 0, 1200, 800);
	//�����ı������ۣ���������Ĵ�С����ɫ����ť�ı���ɫ
	ProQueueMessage->setStyleSheet("QLabel{font:20px;color:#987165;background-color:#F3ECE0;}");
	//�����ı���Ҫ��ʾ����������
	ProQueueMessage->setText(QString::fromStdString(myDisk.showDisk()));//��disk����
	qDebug() << QString::fromStdString(myDisk.showDisk());
	this->show();
}
