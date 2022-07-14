#include "inputwidget.h"

extern FileSystem myFileSystem;
extern PROCESS myPS;
extern memoryManager myMemoryManager;


InputWidget::InputWidget(InputType type)
{

	//switch分支判断窗口类型
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

//创建目录
void InputWidget::CreateDirIU()
{
	//第一步，设置窗口
	this->setFixedSize(500, 300);
	this->setWindowTitle("Create Dir");
	this->ShouldMind = new QLabel("Under Current Path", this);
	ShouldMind->clear();
	ShouldMind->setText("Under Current Path");
	ShouldMind->setFont(QFont("宋体", 10));
	ShouldMind->move(100, 80);
	ShouldMind->adjustSize();


	//第二步，设置输入框
	this->NewDirLineEdit = new QLineEdit("", this);

	//指定输入框位于父窗口中的位置
	NewDirLineEdit->move(100, 100);
	//设置提示信息
	NewDirLineEdit->setPlaceholderText("InputNewDir...");
	//让输入框显示“一键清除”按钮
	NewDirLineEdit->setClearButtonEnabled(true);
	//显示文字大小
	NewDirLineEdit->setFont(QFont("宋体", 10));


	//第三步，收回用户输入的信息
	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(this->width() - this->Confirm->width() - 10, this->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");

	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		//1.验证输入
		bool inValid = this->check(NewDirLineEdit->text(), "another");
		if (!inValid) return;

		//2.验证通过后传给其他模块
		QString NewDir = this->AddNewDir(NewDirLineEdit->text());
		string tmp = NewDir.toStdString();
		myFileSystem.creatFile(tmp);//调用文件部分函数
		emit this->accept();
		});

}


//修改目录
void InputWidget::ModifyDirIU()
{
	//第一步，设置窗口
	this->setFixedSize(500, 300);
	this->setWindowTitle("Change Route");

	//第二步，设置输入框
	this->NewPathLineEdit = new QLineEdit("", this);

	//指定输入框位于父窗口中的位置
	NewPathLineEdit->move(100, 100);
	//设置提示信息
	NewPathLineEdit->setPlaceholderText("ChangeRouteTo...");
	//让输入框显示“一键清除”按钮
	NewPathLineEdit->setClearButtonEnabled(true);
	//显示文字大小
	NewPathLineEdit->setFont(QFont("宋体", 10));


	//第三步，收回用户输入的信息
	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(this->width() - this->Confirm->width() - 10, this->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");


	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		//1.验证输入
		//bool inValid = this->check(NewDirLineEdit->text(), "path");
		//if (!inValid) return;

		//2.验证通过后传给其他模块
		QString NewPath = this->AddNewPath(this->NewPathLineEdit->text());
		string tmp = NewPath.toStdString();
		myFileSystem.changeRoute(tmp);//与文件部分交互修改当前目录//√修改成功已验证
		emit this->accept();
		});
}

void InputWidget::CreateFileIU()
{
	//第一步，设置窗口
	this->setFixedSize(500, 300);
	this->setWindowTitle("Create File");
	this->ShouldMind = new QLabel("在当前目录下", this);
	ShouldMind->clear();
	ShouldMind->setText("Under Current Path");
	ShouldMind->setFont(QFont("宋体", 10));
	ShouldMind->move(100, 80);
	ShouldMind->adjustSize();


	//第二步，设置输入框
	this->NewFileLineEdit = new QLineEdit("", this);

	NewFileLineEdit->clear();
	//指定输入框位于父窗口中的位置
	NewFileLineEdit->move(100, 100);
	//设置提示信息
	NewFileLineEdit->setPlaceholderText("InputNewFileName...");
	//让输入框显示“一键清除”按钮
	NewFileLineEdit->setClearButtonEnabled(true);
	//显示文字大小
	NewFileLineEdit->setFont(QFont("宋体", 10));


	//第三步，收回用户输入的信息
	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(this->width() - this->Confirm->width() - 10, this->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");

	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		//1.验证输入
		bool inValid = this->check(NewFileLineEdit->text(), "another");
		if (!inValid) return;

		//2.验证通过后传给其他模块
		QString NewFile = this->AddNewFile(NewFileLineEdit->text());
		string tmp = NewFile.toStdString();
		int returnNum = myFileSystem.creatFile(tmp);//调用文件部分函数
		if (returnNum == -2) {
			//弹出窗口说明文件名非法
			QMessageBox warn;
			warn.setWindowTitle("Warning");
			warn.setIcon(QMessageBox::Warning);
			warn.setStandardButtons(QMessageBox::Ok);
			warn.setButtonText(QMessageBox::Ok, QString("Accept"));
			warn.setText("Failed to create file: illegal File Name! \nPlease try again:(");
			warn.exec();
		}
		else if (returnNum == -1) {
			//弹出窗口说明文件名重复
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
	//判断是否为空
	if (temp == "")
	{
		warn.setText(pat + "Input Cannot be Null");
		warn.exec();
		return false;
	}
	/*
	//判断输入目录/文件是否存在
	if (pat == "path" && "这里是路径的QStringList.contains(temp)")
	{
		warn.setText("Input" + pat + temp + "Dont Exist,Try Again");
		warn.exec();
		return false;
	}
	if (pat == "file" && "这里是文件的QStringList.contains(temp)")
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
	//第一步，创建树形窗口
	this->TreeWidget = new QTreeWidget(this);
	TreeWidget->setFixedSize(1200, 800);
	this->setWindowTitle("Run File");

	//设置列数
	TreeWidget->setColumnCount(2);
	QStringList HeaderLabel;
	HeaderLabel << "FileName" << "Priority";
	TreeWidget->setHeaderLabels(HeaderLabel);

	//第二步，添加顶层节点和子节点
	this->topItem = new QTreeWidgetItem(TreeWidget);
	topItem->setText(0, "Under Current Path");
	TreeWidget->addTopLevelItem(topItem);

	/*
	//循环放入当前目录下可执行文件
	QStringList a[6];
	a[0] << "a.e";
	a[1] << "b.e";
	this->childItem[0] = new QTreeWidgetItem(this->topItem, a[0]);
	childItem[0]->setCheckState(0, Qt::Unchecked);
	this->childItem[1] = new QTreeWidgetItem(this->topItem, a[1]);
	childItem[1]->setCheckState(0, Qt::Unchecked);
	//TreeWidget->setItemWidget(childItem[1], 1, NewFileLineEdit);
	*/

	//循环放入当前目录下文件
	vector<string> filesVec = myFileSystem.printFile("e");//从文件处得到当前目录的文件
	int num = filesVec.size();//得到文件树
	QStringList a[MAX_FILE_NUM_UNDER_DIR];
	for (int i = 0; i < num; i++) {
		string tmp = filesVec.back();
		filesVec.pop_back();
		QString tmpQ = QString::fromStdString(tmp);//将文件名转换为QString
		a[i] << tmpQ;
		qDebug() << tmpQ;
		this->childItem[i] = new QTreeWidgetItem(this->topItem, a[i]);
		childItem[i]->setCheckState(0, Qt::Unchecked);
		//TreeWidget->setItemWidget(childItem[i], 1, NewFileLineEdit);
	}

	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(TreeWidget->width() - this->Confirm->width() - 10, TreeWidget->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");
	//第三步，点击子节点对文件进行操作，输入文本框内需要用户填入优先级
	connect(TreeWidget, &QTreeWidget::itemClicked, this, &InputWidget::InputPriority);
	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		//this->NewFile = item->text(0);//zmy:传入一个NewFile
		this->NewFileMap.insert(GetNewDir(), NewFileLineEdit->text());
		emit this->accept();
		});
}

void InputWidget::InputPriority(QTreeWidgetItem* item)
{
	this->NewFileLineEdit = new QLineEdit("", this);
	NewFileLineEdit->setPlaceholderText("Input Priority :0/1/2");
	NewFileLineEdit->setFont(QFont("宋体", 10));
	NewFileLineEdit->adjustSize();

	this->TreeWidget->setItemWidget(item, 1, NewFileLineEdit);
	//qDebug() << "Priority Now is:" << NewFileLineEdit->text();

	//传入<文件名，优先级>
	QString itemName = AddNewDir(item->text(0));
	this->NewFile = item->text(0);//zmy:传入一个NewFile
	//this->NewFileMap.insert(item->text(0), NewFileLineEdit->text());


}

void InputWidget::DeleteFileIU()
{
	//第一步，创建树形窗口
	this->TreeWidget = new QTreeWidget(this);
	TreeWidget->setFixedSize(1200, 800);
	this->setWindowTitle("Delete File");

	//设置列数
	TreeWidget->setColumnCount(1);
	QStringList HeaderLabel;
	HeaderLabel << "FileName";
	TreeWidget->setHeaderLabels(HeaderLabel);

	//第二步，添加顶层节点和子节点
	this->topItem = new QTreeWidgetItem(TreeWidget);
	topItem->setText(0, "Under Current Path");
	TreeWidget->addTopLevelItem(topItem);

	//循环放入当前目录下文件
	vector<string> filesVec = myFileSystem.printFile("eft");//从文件处得到当前目录的文件
	int num = filesVec.size();//得到文件树
	QStringList a[MAX_FILE_NUM_UNDER_DIR];
	for (int i = 0; i < num; i++) {
		string tmp = filesVec.back();
		filesVec.pop_back();
		QString tmpQ = QString::fromStdString(tmp);//将文件名转换为QString
		a[i] << tmpQ;
		qDebug() << tmpQ;
		this->childItem[i] = new QTreeWidgetItem(this->topItem, a[i]);
		childItem[i]->setCheckState(0, Qt::Unchecked);
	}

	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(TreeWidget->width() - this->Confirm->width() - 10, TreeWidget->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");
	//第三步，点击子节点对文件进行操作
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

	//将要删除的文件名传给其他模块
	QString DeleteFileName = AddNewFile(item->text(0));
	int returnNum = myFileSystem.deleteFile(DeleteFileName.toStdString());//调用文件系统删除文件
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
	//第一步，创建树形窗口
	this->TreeWidget = new QTreeWidget(this);
	TreeWidget->setFixedSize(1200, 800);
	this->setWindowTitle("Read File");

	//设置列数
	TreeWidget->setColumnCount(1);
	QStringList HeaderLabel;
	HeaderLabel << "FileName";
	TreeWidget->setHeaderLabels(HeaderLabel);

	//第二步，添加顶层节点和子节点
	this->topItem = new QTreeWidgetItem(TreeWidget);
	topItem->setText(0, "Under Current Path");
	TreeWidget->addTopLevelItem(topItem);

	/*
	//循环放入当前目录下可读文件
	QStringList a[6];
	a[0] << "张明昱";
	a[1] << "张博";
	this->childItem[0] = new QTreeWidgetItem(this->topItem, a[0]);
	childItem[0]->setCheckState(0, Qt::Unchecked);

	this->childItem[1] = new QTreeWidgetItem(this->topItem, a[1]);
	childItem[1]->setCheckState(0, Qt::Unchecked);
	*/

	//循环放入当前目录下文件
	vector<string> filesVec = myFileSystem.printFile("et");//从文件处得到当前目录的文件
	int num = filesVec.size();//得到文件树
	QStringList a[MAX_FILE_NUM_UNDER_DIR];
	for (int i = 0; i < num; i++) {
		string tmp = filesVec.back();
		filesVec.pop_back();
		QString tmpQ = QString::fromStdString(tmp);//将文件名转换为QString
		a[i] << tmpQ;
		qDebug() << tmpQ;
		this->childItem[i] = new QTreeWidgetItem(this->topItem, a[i]);
		childItem[i]->setCheckState(0, Qt::Unchecked);
	}

	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(TreeWidget->width() - this->Confirm->width() - 10, TreeWidget->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");
	//第三步，点击子节点对文件进行操作
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
	//第一步，创建树形窗口
	this->TreeWidget = new QTreeWidget(this);
	TreeWidget->setFixedSize(1200, 800);
	this->setWindowTitle("Write File");

	//设置列数
	TreeWidget->setColumnCount(1);
	QStringList HeaderLabel;
	HeaderLabel << "FileName";
	TreeWidget->setHeaderLabels(HeaderLabel);

	//第二步，添加顶层节点和子节点
	this->topItem = new QTreeWidgetItem(TreeWidget);
	topItem->setText(0, "Under Current Path");
	TreeWidget->addTopLevelItem(topItem);

	/*
	//循环放入当前目录下可写文件
	QStringList a[6];
	a[0] << "张明昱";
	a[1] << "张博";
	this->childItem[0] = new QTreeWidgetItem(this->topItem, a[0]);
	childItem[0]->setCheckState(0, Qt::Unchecked);
	this->childItem[1] = new QTreeWidgetItem(this->topItem, a[1]);
	childItem[1]->setCheckState(0, Qt::Unchecked);
	*/
	vector<string> filesVec = myFileSystem.printFile("et");//从文件处得到当前目录的文件
	int num = filesVec.size();//得到文件树
	QStringList a[MAX_FILE_NUM_UNDER_DIR];
	for (int i = 0; i < num; i++) {
		string tmp = filesVec.back();
		filesVec.pop_back();
		QString tmpQ = QString::fromStdString(tmp);//将文件名转换为QString
		a[i] << tmpQ;
		qDebug() << tmpQ;
		this->childItem[i] = new QTreeWidgetItem(this->topItem, a[i]);
		childItem[i]->setCheckState(0, Qt::Unchecked);
	}

	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(TreeWidget->width() - this->Confirm->width() - 10, TreeWidget->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");


	//第三步，点击子节点对文件进行操作
	connect(TreeWidget, &QTreeWidget::itemClicked, this, &InputWidget::InputFileName);
	connect(this->Confirm, &QPushButton::clicked, this, [&]() {
		//writeFileName =
		emit this->accept();
		});
}

void InputWidget::WriteSomethingIU()
{
	//第一步，设置窗口
	this->setFixedSize(1200, 800);
	this->setWindowTitle("Write Something");


	//第二步，设置输入框
	this->NewWriteTextEdit = new QTextEdit("", this);

	//指定输入框位于父窗口中的位置
	NewWriteTextEdit->move(10, 10);
	//显示文字大小
	NewWriteTextEdit->setFont(QFont("宋体", 10));
	NewWriteTextEdit->setFixedSize(1000, 700);

	//第三步，收回用户输入的信息
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
	//第一步，设置窗口
	this->setFixedSize(500, 300);
	this->setWindowTitle("Add Device");

	//第二步，设置输入框
	this->NewFileLineEdit = new QLineEdit("", this);

	NewFileLineEdit->clear();
	//指定输入框位于父窗口中的位置
	NewFileLineEdit->move(100, 100);
	//设置提示信息
	NewFileLineEdit->setPlaceholderText("InputDeviceName...");
	//让输入框显示“一键清除”按钮
	NewFileLineEdit->setClearButtonEnabled(true);
	//显示文字大小
	NewFileLineEdit->setFont(QFont("宋体", 10));


	//第三步，收回用户输入的信息
	this->Confirm = new Button(this, "Accept");
	this->Confirm->move(this->width() - this->Confirm->width() - 10, this->height() - this->Confirm->height() - 10);
	this->Confirm->setStyleSheet("Button{background-color:#FFB5C5;};");

	connect(this->Confirm, &QPushButton::clicked, this, [&]() {

		//1.验证输入(设备有限制)
//        bool inValid = this->check(NewFileLineEdit->text(), "another");
//        if(!inValid) return;

		//2.验证通过后传给其他模块
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
