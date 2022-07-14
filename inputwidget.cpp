#include "inputwidget.h"

extern FileSystem myFileSystem;
extern PROCESS myPS;
extern memoryManager myMemoryManager;


InputWidget::InputWidget(InputType type)
{

	//switch��֧�жϴ�������
	switch (type) {
	case Create_Dir:
		this->CreateDirIU();
		break;

	case Modify_Dir:
		this->ModifyDirIU();
		break;

	case Create_File:
		this->CreateFileIU();
		break;

	case Run_File:
		this->RunFileIU();
		break;

	case Del_File:
		this->DeleteFileIU();
		break;

	case Read_File:
		this->ReadFileIU();
		break;

	case Write_File:
		this->WriteFileIU();
		break;

	case Write_Something:
		this->WriteSomethingIU();
		break;

	case Add_Device:
		this->AddDeviceUI();
		break;
	}

}

//����Ŀ¼
void InputWidget::CreateDirIU()
{
	//��һ�������ô���
	this->setFixedSize(500, 300);
	this->setWindowTitle("Create Dir");
	this->ShouldMind = new QLabel("Under Current Path", this);
	ShouldMind->clear();
	ShouldMind->setText("Under Current Path");
	ShouldMind->setFont(QFont("����", 10));
	ShouldMind->move(100, 80);
	ShouldMind->adjustSize();


	//�ڶ��������������
	this->NewDirLineEdit = new QLineEdit("", this);

	//ָ�������λ�ڸ������е�λ��
	NewDirLineEdit->move(100, 100);
	//������ʾ��Ϣ
	NewDirLineEdit->setPlaceholderText("InputNewDir...");
	//���������ʾ��һ���������ť
	NewDirLineEdit->setClearButtonEnabled(true);
	//��ʾ���ִ�С
	NewDirLineEdit->setFont(QFont("����", 10));


	//���������ջ��û��������Ϣ
	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(this->width() - this->Confirm->width() - 10, this->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");

	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		//1.��֤����
		bool inValid = this->check(NewDirLineEdit->text(), "another");
		if (!inValid) return;

		//2.��֤ͨ���󴫸�����ģ��
		QString NewDir = this->AddNewDir(NewDirLineEdit->text());
		string tmp = NewDir.toStdString();
		myFileSystem.creatFile(tmp);//�����ļ����ֺ���
		emit this->accept();
		});

}


//�޸�Ŀ¼
void InputWidget::ModifyDirIU()
{
	//��һ�������ô���
	this->setFixedSize(500, 300);
	this->setWindowTitle("Change Route");

	//�ڶ��������������
	this->NewPathLineEdit = new QLineEdit("", this);

	//ָ�������λ�ڸ������е�λ��
	NewPathLineEdit->move(100, 100);
	//������ʾ��Ϣ
	NewPathLineEdit->setPlaceholderText("ChangeRouteTo...");
	//���������ʾ��һ���������ť
	NewPathLineEdit->setClearButtonEnabled(true);
	//��ʾ���ִ�С
	NewPathLineEdit->setFont(QFont("����", 10));


	//���������ջ��û��������Ϣ
	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(this->width() - this->Confirm->width() - 10, this->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");


	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		//1.��֤����
		//bool inValid = this->check(NewDirLineEdit->text(), "path");
		//if (!inValid) return;

		//2.��֤ͨ���󴫸�����ģ��
		QString NewPath = this->AddNewPath(this->NewPathLineEdit->text());
		string tmp = NewPath.toStdString();
		myFileSystem.changeRoute(tmp);//���ļ����ֽ����޸ĵ�ǰĿ¼//���޸ĳɹ�����֤
		emit this->accept();
		});
}

void InputWidget::CreateFileIU()
{
	//��һ�������ô���
	this->setFixedSize(500, 300);
	this->setWindowTitle("Create File");
	this->ShouldMind = new QLabel("�ڵ�ǰĿ¼��", this);
	ShouldMind->clear();
	ShouldMind->setText("Under Current Path");
	ShouldMind->setFont(QFont("����", 10));
	ShouldMind->move(100, 80);
	ShouldMind->adjustSize();


	//�ڶ��������������
	this->NewFileLineEdit = new QLineEdit("", this);

	NewFileLineEdit->clear();
	//ָ�������λ�ڸ������е�λ��
	NewFileLineEdit->move(100, 100);
	//������ʾ��Ϣ
	NewFileLineEdit->setPlaceholderText("InputNewFileName...");
	//���������ʾ��һ���������ť
	NewFileLineEdit->setClearButtonEnabled(true);
	//��ʾ���ִ�С
	NewFileLineEdit->setFont(QFont("����", 10));


	//���������ջ��û��������Ϣ
	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(this->width() - this->Confirm->width() - 10, this->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");

	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		//1.��֤����
		bool inValid = this->check(NewFileLineEdit->text(), "another");
		if (!inValid) return;

		//2.��֤ͨ���󴫸�����ģ��
		QString NewFile = this->AddNewFile(NewFileLineEdit->text());
		string tmp = NewFile.toStdString();
		int returnNum = myFileSystem.creatFile(tmp);//�����ļ����ֺ���
		if (returnNum == -2) {
			//��������˵���ļ����Ƿ�
			QMessageBox warn;
			warn.setWindowTitle("Warning");
			warn.setIcon(QMessageBox::Warning);
			warn.setStandardButtons(QMessageBox::Ok);
			warn.setButtonText(QMessageBox::Ok, QString("Accept"));
			warn.setText("Failed to create file: illegal File Name! \nPlease try again:(");
			warn.exec();
		}
		else if (returnNum == -1) {
			//��������˵���ļ����ظ�
			QMessageBox warn;
			warn.setWindowTitle("Warning");
			warn.setIcon(QMessageBox::Warning);
			warn.setStandardButtons(QMessageBox::Ok);
			warn.setButtonText(QMessageBox::Ok, QString("Accept"));
			warn.setText("Failed to create file: File Name already exsits! \nPlease try again:(");
			warn.exec();

		}
		emit this->accept();

		});
}



bool InputWidget::check(QString inputString, QString pattern)
{
	QString temp = inputString;
	QString pat = pattern;

	QMessageBox warn;
	warn.setWindowTitle("Warning");
	warn.setIcon(QMessageBox::Warning);
	warn.setStandardButtons(QMessageBox::Ok);
	warn.setButtonText(QMessageBox::Ok, QString("Accept"));
	//�ж��Ƿ�Ϊ��
	if (temp == "")
	{
		warn.setText(pat + "Input Cannot be Null");
		warn.exec();
		return false;
	}
	/*
	//�ж�����Ŀ¼/�ļ��Ƿ����
	if (pat == "path" && "������·����QStringList.contains(temp)")
	{
		warn.setText("Input" + pat + temp + "Dont Exist,Try Again");
		warn.exec();
		return false;
	}
	if (pat == "file" && "�������ļ���QStringList.contains(temp)")
	{
		warn.setText("Input" + pat + temp + "Dont Exist,Try Again");
		warn.exec();
		return false;
	}
	*/
	return true;
}


void InputWidget::RunFileIU()
{
	//��һ�����������δ���
	this->TreeWidget = new QTreeWidget(this);
	TreeWidget->setFixedSize(1200, 800);
	this->setWindowTitle("Run File");

	//��������
	TreeWidget->setColumnCount(2);
	QStringList HeaderLabel;
	HeaderLabel << "FileName" << "Priority";
	TreeWidget->setHeaderLabels(HeaderLabel);

	//�ڶ�������Ӷ���ڵ���ӽڵ�
	this->topItem = new QTreeWidgetItem(TreeWidget);
	topItem->setText(0, "Under Current Path");
	TreeWidget->addTopLevelItem(topItem);

	/*
	//ѭ�����뵱ǰĿ¼�¿�ִ���ļ�
	QStringList a[6];
	a[0] << "a.e";
	a[1] << "b.e";
	this->childItem[0] = new QTreeWidgetItem(this->topItem, a[0]);
	childItem[0]->setCheckState(0, Qt::Unchecked);
	this->childItem[1] = new QTreeWidgetItem(this->topItem, a[1]);
	childItem[1]->setCheckState(0, Qt::Unchecked);
	//TreeWidget->setItemWidget(childItem[1], 1, NewFileLineEdit);
	*/

	//ѭ�����뵱ǰĿ¼���ļ�
	vector<string> filesVec = myFileSystem.printFile("e");//���ļ����õ���ǰĿ¼���ļ�
	int num = filesVec.size();//�õ��ļ���
	QStringList a[MAX_FILE_NUM_UNDER_DIR];
	for (int i = 0; i < num; i++) {
		string tmp = filesVec.back();
		filesVec.pop_back();
		QString tmpQ = QString::fromStdString(tmp);//���ļ���ת��ΪQString
		a[i] << tmpQ;
		qDebug() << tmpQ;
		this->childItem[i] = new QTreeWidgetItem(this->topItem, a[i]);
		childItem[i]->setCheckState(0, Qt::Unchecked);
		//TreeWidget->setItemWidget(childItem[i], 1, NewFileLineEdit);
	}

	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(TreeWidget->width() - this->Confirm->width() - 10, TreeWidget->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");
	//������������ӽڵ���ļ����в����������ı�������Ҫ�û��������ȼ�
	connect(TreeWidget, &QTreeWidget::itemClicked, this, &InputWidget::InputPriority);
	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		//this->NewFile = item->text(0);//zmy:����һ��NewFile
		this->NewFileMap.insert(GetNewDir(), NewFileLineEdit->text());
		emit this->accept();
		});
}

void InputWidget::InputPriority(QTreeWidgetItem* item)
{
	this->NewFileLineEdit = new QLineEdit("", this);
	NewFileLineEdit->setPlaceholderText("Input Priority :0/1/2");
	NewFileLineEdit->setFont(QFont("����", 10));
	NewFileLineEdit->adjustSize();

	this->TreeWidget->setItemWidget(item, 1, NewFileLineEdit);
	//qDebug() << "Priority Now is:" << NewFileLineEdit->text();

	//����<�ļ��������ȼ�>
	QString itemName = AddNewDir(item->text(0));
	this->NewFile = item->text(0);//zmy:����һ��NewFile
	//this->NewFileMap.insert(item->text(0), NewFileLineEdit->text());


}

void InputWidget::DeleteFileIU()
{
	//��һ�����������δ���
	this->TreeWidget = new QTreeWidget(this);
	TreeWidget->setFixedSize(1200, 800);
	this->setWindowTitle("Delete File");

	//��������
	TreeWidget->setColumnCount(1);
	QStringList HeaderLabel;
	HeaderLabel << "FileName";
	TreeWidget->setHeaderLabels(HeaderLabel);

	//�ڶ�������Ӷ���ڵ���ӽڵ�
	this->topItem = new QTreeWidgetItem(TreeWidget);
	topItem->setText(0, "Under Current Path");
	TreeWidget->addTopLevelItem(topItem);

	//ѭ�����뵱ǰĿ¼���ļ�
	vector<string> filesVec = myFileSystem.printFile("eft");//���ļ����õ���ǰĿ¼���ļ�
	int num = filesVec.size();//�õ��ļ���
	QStringList a[MAX_FILE_NUM_UNDER_DIR];
	for (int i = 0; i < num; i++) {
		string tmp = filesVec.back();
		filesVec.pop_back();
		QString tmpQ = QString::fromStdString(tmp);//���ļ���ת��ΪQString
		a[i] << tmpQ;
		qDebug() << tmpQ;
		this->childItem[i] = new QTreeWidgetItem(this->topItem, a[i]);
		childItem[i]->setCheckState(0, Qt::Unchecked);
	}

	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(TreeWidget->width() - this->Confirm->width() - 10, TreeWidget->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");
	//������������ӽڵ���ļ����в���
	connect(TreeWidget, &QTreeWidget::itemClicked, this, &InputWidget::CheckInputDelete);
	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		emit this->accept();
		});
}

void InputWidget::CheckInputDelete(QTreeWidgetItem* item)
{
	QMessageBox warn;
	warn.setWindowTitle("Warning");
	warn.setIcon(QMessageBox::Warning);
	warn.setStandardButtons(QMessageBox::Ok);
	warn.setButtonText(QMessageBox::Ok, QString("Accept"));
	warn.setText("Are U Sure to Delete the File?");
	warn.exec();

	//��Ҫɾ�����ļ�����������ģ��
	QString DeleteFileName = AddNewFile(item->text(0));
	int returnNum = myFileSystem.deleteFile(DeleteFileName.toStdString());//�����ļ�ϵͳɾ���ļ�
	if (returnNum == -1) {
		QMessageBox warn;
		warn.setWindowTitle("Warning");
		warn.setIcon(QMessageBox::Warning);
		warn.setStandardButtons(QMessageBox::Ok);
		warn.setButtonText(QMessageBox::Ok, QString("Accept"));
		warn.setText("Failed to delete file: not allowed to delete! \nPlease try again:(");
		warn.exec();
	}
	if (returnNum == -2) {
		QMessageBox warn;
		warn.setWindowTitle("Warning");
		warn.setIcon(QMessageBox::Warning);
		warn.setStandardButtons(QMessageBox::Ok);
		warn.setButtonText(QMessageBox::Ok, QString("Accept"));
		warn.setText("Failed to delete file: the file is being used now! \nPlease try later:(");
		warn.exec();
	}
}

void InputWidget::ReadFileIU()
{
	//��һ�����������δ���
	this->TreeWidget = new QTreeWidget(this);
	TreeWidget->setFixedSize(1200, 800);
	this->setWindowTitle("Read File");

	//��������
	TreeWidget->setColumnCount(1);
	QStringList HeaderLabel;
	HeaderLabel << "FileName";
	TreeWidget->setHeaderLabels(HeaderLabel);

	//�ڶ�������Ӷ���ڵ���ӽڵ�
	this->topItem = new QTreeWidgetItem(TreeWidget);
	topItem->setText(0, "Under Current Path");
	TreeWidget->addTopLevelItem(topItem);

	/*
	//ѭ�����뵱ǰĿ¼�¿ɶ��ļ�
	QStringList a[6];
	a[0] << "������";
	a[1] << "�Ų�";
	this->childItem[0] = new QTreeWidgetItem(this->topItem, a[0]);
	childItem[0]->setCheckState(0, Qt::Unchecked);

	this->childItem[1] = new QTreeWidgetItem(this->topItem, a[1]);
	childItem[1]->setCheckState(0, Qt::Unchecked);
	*/

	//ѭ�����뵱ǰĿ¼���ļ�
	vector<string> filesVec = myFileSystem.printFile("et");//���ļ����õ���ǰĿ¼���ļ�
	int num = filesVec.size();//�õ��ļ���
	QStringList a[MAX_FILE_NUM_UNDER_DIR];
	for (int i = 0; i < num; i++) {
		string tmp = filesVec.back();
		filesVec.pop_back();
		QString tmpQ = QString::fromStdString(tmp);//���ļ���ת��ΪQString
		a[i] << tmpQ;
		qDebug() << tmpQ;
		this->childItem[i] = new QTreeWidgetItem(this->topItem, a[i]);
		childItem[i]->setCheckState(0, Qt::Unchecked);
	}

	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(TreeWidget->width() - this->Confirm->width() - 10, TreeWidget->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");
	//������������ӽڵ���ļ����в���
	connect(TreeWidget, &QTreeWidget::itemClicked, this, &InputWidget::InputFileName);
	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		emit this->accept();
		});
}

void InputWidget::InputFileName(QTreeWidgetItem* item)
{
	QString ReadFileName = AddNewFile(item->text(0));

}

void InputWidget::WriteFileIU()
{
	//��һ�����������δ���
	this->TreeWidget = new QTreeWidget(this);
	TreeWidget->setFixedSize(1200, 800);
	this->setWindowTitle("Write File");

	//��������
	TreeWidget->setColumnCount(1);
	QStringList HeaderLabel;
	HeaderLabel << "FileName";
	TreeWidget->setHeaderLabels(HeaderLabel);

	//�ڶ�������Ӷ���ڵ���ӽڵ�
	this->topItem = new QTreeWidgetItem(TreeWidget);
	topItem->setText(0, "Under Current Path");
	TreeWidget->addTopLevelItem(topItem);

	/*
	//ѭ�����뵱ǰĿ¼�¿�д�ļ�
	QStringList a[6];
	a[0] << "������";
	a[1] << "�Ų�";
	this->childItem[0] = new QTreeWidgetItem(this->topItem, a[0]);
	childItem[0]->setCheckState(0, Qt::Unchecked);
	this->childItem[1] = new QTreeWidgetItem(this->topItem, a[1]);
	childItem[1]->setCheckState(0, Qt::Unchecked);
	*/
	vector<string> filesVec = myFileSystem.printFile("et");//���ļ����õ���ǰĿ¼���ļ�
	int num = filesVec.size();//�õ��ļ���
	QStringList a[MAX_FILE_NUM_UNDER_DIR];
	for (int i = 0; i < num; i++) {
		string tmp = filesVec.back();
		filesVec.pop_back();
		QString tmpQ = QString::fromStdString(tmp);//���ļ���ת��ΪQString
		a[i] << tmpQ;
		qDebug() << tmpQ;
		this->childItem[i] = new QTreeWidgetItem(this->topItem, a[i]);
		childItem[i]->setCheckState(0, Qt::Unchecked);
	}

	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(TreeWidget->width() - this->Confirm->width() - 10, TreeWidget->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");


	//������������ӽڵ���ļ����в���
	connect(TreeWidget, &QTreeWidget::itemClicked, this, &InputWidget::InputFileName);
	connect(this->Confirm, &QPushButton::clicked, this, [&]() {
		//writeFileName =
		emit this->accept();
		});
}

void InputWidget::WriteSomethingIU()
{
	//��һ�������ô���
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Write Something");


	//�ڶ��������������
	this->NewWriteTextEdit = new QTextEdit("", this);

	//ָ�������λ�ڸ������е�λ��
	NewWriteTextEdit->move(10, 10);
	//��ʾ���ִ�С
	NewWriteTextEdit->setFont(QFont("����", 10));
	NewWriteTextEdit->setFixedSize(1000, 700);

	//���������ջ��û��������Ϣ
	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(this->width() - this->Confirm->width() - 10, this->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");


	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		this->AddWriteSomething(NewWriteTextEdit->toPlainText());

		emit this->reaccept();

		});
}

void InputWidget::AddDeviceUI()
{
	//��һ�������ô���
	this->setFixedSize(500, 300);
	this->setWindowTitle("Add Device");

	//�ڶ��������������
	this->NewFileLineEdit = new QLineEdit("", this);

	NewFileLineEdit->clear();
	//ָ�������λ�ڸ������е�λ��
	NewFileLineEdit->move(100, 100);
	//������ʾ��Ϣ
	NewFileLineEdit->setPlaceholderText("InputDeviceName...");
	//���������ʾ��һ���������ť
	NewFileLineEdit->setClearButtonEnabled(true);
	//��ʾ���ִ�С
	NewFileLineEdit->setFont(QFont("����", 10));


	//���������ջ��û��������Ϣ
	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(this->width() - this->Confirm->width() - 10, this->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");

	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		//1.��֤����(�豸������)
//        bool inValid = this->check(NewFileLineEdit->text(), "another");
//        if(!inValid) return;

		//2.��֤ͨ���󴫸�����ģ��
		QString NewFile = this->AddNewFile(NewFileLineEdit->text());

		emit this->accept();

		});
}


QString InputWidget::AddNewDir(QString NewDir)
{
	this->NewDir = NewDir;
	return this->NewDir;
}
QString InputWidget::GetNewDir()
{
	return this->NewDir;
}

QString InputWidget::AddNewPath(QString NewPath)
{
	this->NewPath = NewPath;
	return this->NewPath;
}
QString InputWidget::GetNewPath()
{
	return this->NewPath;
}

QString InputWidget::AddNewFile(QString NewFile)
{
	this->NewFile = NewFile;
	return this->NewFile;
}
QString InputWidget::GetNewFile()
{
	return this->NewFile;
}

void InputWidget::AddWriteSomething(QString something)
{
	this->something = something;
}
QString InputWidget::GetWriteSomething()
{
	return this->something;
}
