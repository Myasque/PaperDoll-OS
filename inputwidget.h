#pragma once
#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H
#define MAX_FILE_NUM_UNDER_DIR 100 //在目录下的最大文件数
#include<string>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QDebug>
#include <QMessageBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include<stddef.h>
#include <QMap>
#include "button.h"
#include"PROCESS.h"


enum InputType
{
	Create_Dir,
	Modify_Dir,
	Create_File,
	Run_File,
	Del_File,
	Read_File,
	Write_File,
	Write_Something,
	Add_Device
};


class InputWidget : public QDialog
{
	Q_OBJECT
public:
	InputWidget(InputType type);

public:
	//变量
	QLabel* ShouldMind;//提示在当前目录下进行操作

	QLineEdit* NewDirLineEdit;//新目录输入框

	QLineEdit* NewPathLineEdit;//当前路径

	QLineEdit* NewFileLineEdit;//新文件输入框

	QTextEdit* NewWriteTextEdit;//写入文件

	QTreeWidget* TreeWidget;
	QTreeWidgetItem* topItem;
	//    QList<QTreeWidgetItem *> *items;
	QTreeWidgetItem* childItem[100];//zmy try


	Button* Confirm;//输入框确认按钮

private:
	//传给其他
	QString PassToDevice;

	QString NewDir;

	QString NewPath;

	QString NewFile;

	QString something;

public:
	//函数

	void CreateDirIU();

	void ModifyDirIU();

	void CreateFileIU();

	void DeleteFileIU();

	void RunFileIU();

	void ReadFileIU();

	void WriteFileIU();

	void WriteSomethingIU();

	bool check(QString inputString, QString pattern);

	void AddDeviceUI();

	//传给其他
	QString GetMessagePassToDevice();

	QString AddNewDir(QString NewDir);
	QString GetNewDir();

	QString AddNewPath(QString NewPath);
	QString GetNewPath();

	QString AddNewFile(QString NewFile);
	QString GetNewFile();

	QMap <QString, QString> NewFileMap;

	void AddWriteSomething(QString something);
	QString GetWriteSomething();

public slots:
	void InputPriority(QTreeWidgetItem* item);

	void CheckInputDelete(QTreeWidgetItem* item);

	void InputFileName(QTreeWidgetItem* item);

signals:
	void accept();
	void reaccept();
};

#endif // INPUTWIDGET_H
