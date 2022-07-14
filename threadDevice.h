#pragma once
#include <QThread>

class ThreadDevice : public QThread
{
public:
	ThreadDevice();
	void closeThread();

protected:
	virtual void run();//主要运行函数

private:
	volatile bool isStop;       //isStop是易失性变量，需要用volatile进行申明
};

