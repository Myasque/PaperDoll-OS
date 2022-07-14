#ifndef MAINSCENE_H
#define MAINSCENE_H

//�˽���Ϊ�����棬��ʾ���еķ���
#include <QWidget>
#include<QString>
#include <QLCDNumber>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QLabel>
#include <QFile>
#include <QVBoxLayout>
#include"mythread.h"
#include"threadDevice.h"
#include "button.h"
#include "inputwidget.h"
#include "outputwidget.h"
#include"FileSystem.h"
#include "threadDevice.h"

#define BUTTONNUM 18

struct SystemTime
{
	int hour;
	int min;
	int second;
};

class MainScene : public QWidget
{
	Q_OBJECT

private:
	MyThread* thread1;

	ThreadDevice* thread2;

public://����
	explicit MainScene(QWidget* parent = nullptr);//�����������ʼ����

	void init();//��ʼ��

	void setPageLayout();//���ô��ںͲ���

	void updateUI();


public://����

	/*������*/
//    Button *mybutton[BUTTONNUM];
	Button* proInfo;//���̲���
	Button* proQueue;//���̶���
	Button* synShow;//ͬ����ʾ
	Button* suspend;//�ж�

	Button* fileTree;//�ļ���
	Button* createContent;//����Ŀ¼
	Button* modifyContent;//�޸�Ŀ¼
	Button* createFile;//�����ļ�
	Button* runFile;//�����ļ�
	Button* deleteFile;//ɾ���ļ�
	Button* readFile;//��ȡ�ļ�
	Button* writeFile;//д���ļ�

	Button* diskIfo;//�����Դ
	Button* memoryUsage;//�ڴ�ռ�����
	Button* diskUsage;//����ռ�����
	Button* deviceInfo;//�豸״̬
	Button* addDevice;//����豸

	Button* backBtns;//���ذ�ť

	QString nowPathString;
	QLabel* nowPath;//��ǰ����·��

	QLabel* OutputSys;//ͬ����ʾ

	InputWidget* iw;//��ʾ���봰��

	OutputWidget* ow;//��ʾ�������

	InputWidget* iw1;//д���ļ�����

	int flag = 0;
	int flag2 = 0;
	int countSys = 0;
	int countMon = 0;
	int interruptPid;
	QString WriteFileName;//zmy

	QtCharts::QChart* chart;
	QtCharts::QChartView* chartView;

	QtCharts::QLineSeries* dataSeries;

	QtCharts::QValueAxis* axisX;
	QtCharts::QValueAxis* axisY;


	/*ϵͳ��ʱ��*/
//    QLCDNumber *tm_widget;//��ʾʱ�䵱ǰʱ��

//    SystemTime time;//ϵͳʱ��




public slots:

	void ShowProInfo();//չʾ���̲���

	void ShowProQueue();//չʾ���̶���

	void ShowSys();//ͬ����ʾ

	void InterrputProcess();//�жϽ���

	void ShowFileTree();//�ļ���

	void CreateDir();//����Ŀ¼

	void ModifyDir();//�޸�Ŀ¼

	void CreateFile();//�����ļ�

	void RunFile();//�����ļ�

	void DeleteFile();//ɾ���ļ�

	void ReadFile();//��ȡ�ļ�

	void WriteFile();//д���ļ�

	void DiskIfo();//��ش���

	void MemoryUsage();//�ڴ�ռ�����

	void DiskUsage();//����ռ�����

	void DeviceStatus();//չʾ�豸״̬

	void AddDevice();//����豸

	void openThreadBtnSlot();//zmy

signals:
	void startThread();//zmy
	void backToWidget();

};

#endif // MAINSCENE_H
