#pragma once
#include <QThread>

class ThreadDevice : public QThread
{
public:
	ThreadDevice();
	void closeThread();

protected:
	virtual void run();//��Ҫ���к���

private:
	volatile bool isStop;       //isStop����ʧ�Ա�������Ҫ��volatile��������
};

