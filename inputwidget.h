#pragma once
#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H
#define MAX_FILE_NUM_UNDER_DIR 100 //��Ŀ¼�µ�����ļ���
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
	//����
	QLabel* ShouldMind;//��ʾ�ڵ�ǰĿ¼�½��в���

	QLineEdit* NewDirLineEdit;//��Ŀ¼�����

	QLineEdit* NewPathLineEdit;//��ǰ·��

	QLineEdit* NewFileLineEdit;//���ļ������

	QTextEdit* NewWriteTextEdit;//д���ļ�

	QTreeWidget* TreeWidget;
	QTreeWidgetItem* topItem;
	//    QList<QTreeWidgetItem *> *items;
	QTreeWidgetItem* childItem[100];//zmy try


	Button* Confirm;//�����ȷ�ϰ�ť

private:
	//��������
	QString PassToDevice;

	QString NewDir;

	QString NewPath;

	QString NewFile;

	QString something;

public:
	//����

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

	//��������
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
