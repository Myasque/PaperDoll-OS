#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>

class MyThread : public QThread
{
public:
	MyThread();
	void closeThread();

protected:
	virtual void run();//��Ҫ���к���

private:
	volatile bool isStop;       //isStop����ʧ�Ա�������Ҫ��volatile��������
};

#endif // MYTHREAD_H