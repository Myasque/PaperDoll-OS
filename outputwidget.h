#pragma once
#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H
#define MAX_FILE_NUM_UNDER_DIR 100 //在目录下的最大文件数
#include <QDialog>
#include <QLabel>
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QTimer>
#include <QDebug>
#include"PROCESS.h"
#include"disk.h"


enum OutputType
{
	Process_info,
	Process_Queue,
	Show_Sys,
	File_Tree,
	Read_Files,
	Memory_Usage,
	Device_Info,
	Disk_output
	//Device_Buffer
};

class OutputWidget : public QDialog
{
	Q_OBJECT
public:
	OutputWidget(OutputType type);

public:
	QLabel* ProInfoMessage;

	QLabel* ProQueueMessage;

	QTreeWidget* TreeWidget;
	QTreeWidgetItem* topItem;

	QTreeWidgetItem* childItem[500];//zmy:*notice*

	int temp = 0;

public:
	void OutputProcessInfo();

	void OutputProcessQueue();

	void OutputSys();

	void OutputFileTree();

	void OutputReadFile();

	void OutputMemoryUsage();

	void OutputDeviceStatus();

	void OutputDisk();//zmy

};

#endif // OUTPUTWIDGET_H

