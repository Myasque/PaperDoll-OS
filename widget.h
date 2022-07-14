#ifndef WIDGET_H
#define WIDGET_H

//此窗口显示刚进入的开始界面，用户可以进入或退出系统
#include <QWidget>
#include <QLabel>
#include <QDebug>
#include <QComboBox>
#include <QInputDialog>
#include "button.h"
#include "mainscene.h"

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget* parent = nullptr);//初始化窗口
	~Widget();

	void on_Item_clicked();

public:
	QLabel* title;//标题（欢迎来到操作系统）

	Button* enter;//进入按钮

	Button* exit;//退出按钮

	MainScene* scene;//主场景的入口

	QString ProcessSchAlgitem;//进程调度算法

	QString MemoryStorageitem;//内存存储方式

	QString HeadSeekAlgitem;//磁头寻道算法

};
#endif // WIDGET_H

