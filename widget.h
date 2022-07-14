#ifndef WIDGET_H
#define WIDGET_H

//�˴�����ʾ�ս���Ŀ�ʼ���棬�û����Խ�����˳�ϵͳ
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
	Widget(QWidget* parent = nullptr);//��ʼ������
	~Widget();

	void on_Item_clicked();

public:
	QLabel* title;//���⣨��ӭ��������ϵͳ��

	Button* enter;//���밴ť

	Button* exit;//�˳���ť

	MainScene* scene;//�����������

	QString ProcessSchAlgitem;//���̵����㷨

	QString MemoryStorageitem;//�ڴ�洢��ʽ

	QString HeadSeekAlgitem;//��ͷѰ���㷨

};
#endif // WIDGET_H

