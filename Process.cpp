#include"PROCESS.h"
#define PAGESIZE 1024
int isMLFQ = 0;  //是否选择MLFQ算法，是=1，否=0
int isSJF = 0;  //是否选择SJF算法，是=1，否=0
int isFCFS = 0;  //是否选择FCFS算法，是=1，否=0
int isPreemptive = 0;;  //是否抢占，是=1，否=0，仅SJF模式下使用
double timeslice0 = 5;   //队列0的时间片
double timeslice1 = 3;  //队列1的时间片
//extern const int pageSize ;  //页面大小
#include <QThread>
extern FileSystem myFileSystem;//文件系统
extern memoryManager myMemoryManager;//内存管理
extern DeviceSystem myDeviceSystem;
extern PROCESS myPS;

/**************************************************************************************************/
/******************************************INTERRUPT.cpp内容***************************************/
/**************************************************************************************************/
/*
void INTERRUPT::interrupt(int pid, InterrupType type)
{
	if (type == TimeSlice)
		interruptTimeSlice(pid);
	else if (type == IO)
		interruptIO(pid);
	else if (type == PageMiss)
		interruptPageMiss(pid);
	else if (type == User)
		interruptUser(pid);
	else
	{
		cout << "错误的中断类型:" << type << "!" << endl;
		exit(0);
	}
}*/


int INTERRUPT::interruptTimeSlice(double timeslice, PCB* pcb)
{
	if (timeslice > 0)
	{
		time_t temp;
		time(&temp);
		qDebug() << "timeslice interrupt ?";
		qDebug() << "difftime=" << difftime(temp, pcb->timeLUCPU) << "and timrslice is " << timeslice;
		if (difftime(temp, pcb->timeLUCPU) >= timeslice) //正在运行的进程时间片用尽
		{
			qDebug() << "your timeslice run out!";
			//Sleep(500);
			myPS.RunningToReady(pcb->pid);
			//myPS.scheduling();
			return 1;
		}
	}
	return 0;
}

void INTERRUPT::interruptIO(int pid, string device)
{
	myPS.RunningToBlock(pid);
	/************* Device **************/
	srand(time(NULL));
	int time = rand() % 5 + 2;
	myDeviceSystem.requestDevice(pid, device, time);

	//myPS.BlockToReady(pid);
}

void INTERRUPT::interruptPageMiss(int pid, int start, int end)
{
	myPS.RunningToBlock(pid);
	myMemoryManager.pageIO(pid, start, end);
	myPS.BlockToReady(pid);
}

void INTERRUPT::interruptUser(PCB* pcb)
{
	qDebug() << "您已手动触发中断！" << endl;
	//<< "当前正在运行的进程信息为：" << endl;
//pcb->outPCB();
}

/**************************************************************************************************/
/******************************************MLFQUEUE.cpp内容****************************************/
/**************************************************************************************************/

int MLFQUEUE::getQueueNum(int num)  //返回队列中的进程数量，参数为队列编号(取值0/1/2)
{
	if (num == 0)
		return queue0.getQueueNum();
	else if (num == 1)
		return queue1.getQueueNum();
	else if (num == 2)
		return queue2.getQueueNum();
}

int MLFQUEUE::getPID(int queueNum, int pos)  //根据进程所在队列号和处在队列中位置(从0始)返回pid
{
	if (queueNum == 0)
		return queue0.getPID(pos);
	else if (queueNum == 1)
		return queue1.getPID(pos);
	else if (queueNum == 2)
		return queue2.getPID(pos);
}

//根据进程优先级，将进程添加到queue0 || queue1 || queue2
void MLFQUEUE::addProcess(int pid, int priority)
{
	switch (priority)
	{
	case 0:
		queue0.addProcess(pid);
		break;
	case 1:
		queue1.addProcess(pid);
		break;
	case 2:
		queue2.addProcess(pid);
		break;
	default:
		qDebug() << "检测到非法的进程优先级—— pid=" << pid << "	priority=" << priority << endl;
		//exit(0);
	}
}

//根据进程优先级，将进程从queue0 || queue1 || queue2中删除
void MLFQUEUE::popProcess(int pid, int priority)
{
	switch (priority)
	{
	case 0:
		queue0.popProcess(pid);
		break;
	case 1:
		queue1.popProcess(pid);
		break;
	case 2:
		queue2.popProcess(pid);
		break;
	default:
		qDebug() << "illegal priority—— pid=" << pid << "	priority=" << priority << endl;
		//exit(0);
	}
}

/**************************************************************************************************/
/******************************************PCB.cpp内容****************************************/
/**************************************************************************************************/
PCB::PCB()
{
	this->pid = 12345;
	this->ppid = -1;//zmy:初始化ppid
	this->state = New;
	this->timeLast = 0;
	this->timeLUCPU = 0;
	this->priority = 2;
	this->PC = 0;
	time_t timep;
	time(&timep);
	timeArrive = timep;
	srand(time(NULL));
	//timeLast = rand() % 10 + 2;
}

PCB::PCB(int pid, int size)
{
	this->pid = pid;
	this->ppid = -1;//zmy:初始化ppid
	this->state = New;
	this->size = size;
	this->priority = 2;
	this->PC = 0;
	time_t timep;
	time(&timep);
	timeArrive = timep;
	srand(time(NULL));
	//timeLast = rand() % 10 + 2;
}

PCB::PCB(int pid, int size, int priority)
{
	this->pid = pid;
	this->ppid = -1;//zmy:初始化ppid
	this->state = New;
	this->size = size;
	this->priority = priority;
	this->PC = 0;
	time_t timep;
	time(&timep);
	timeArrive = timep;
	srand(time(NULL));
	//timeLast = rand() % 10 + 2;
}

//由于进程的状态定义了枚举不能打印字符串，所以用该函数打印状态
string PCB::outState()
{
	switch (this->state)
	{
	case New:
		return "New";
	case Ready:
		return "Ready";
	case Running:
		return "Running";
	case Block:
		return "Block";
	case Terminated:
		return "Terminated";
	default:
		cout << "进程" << this->pid << "状态发生异常！" << this->state << endl;
		//exit(0);
	}
}

//将PCB内所有信息输出
string PCB::outPCB()
{
	string output = "";
	output += "PID : ";
	output += to_string(pid);
	output += "\n";
	output += "state : ";
	output += outState();
	output += "\n";
	output += "IR : ";
	output += IR;
	output += "\n";
	output += "PC : ";
	output += to_string(PC);
	output += "\n";
	//cout << "state:" << outState() << endl;
	//cout << "IR:" << IR << endl;
	//cout << "PC:" << PC << endl;
	char time[26];
	ctime_s(time, sizeof(time), &timeArrive);
	output += "timeArrive : ";
	output += time;
	//output += "\n";
	//cout << "timeArrive:" << time;
	ctime_s(time, sizeof(time), &timeLUCPU);
	output += "timeLUCPU : ";
	output += time;
	output += "\n";
	//cout << "timeLUCPU:" << time;
	output += "timeLast:";
	output += to_string(timeLast);
	output += "\n";
	//cout << "timeLast:" << timeLast << endl;
	output += "priority : ";
	output += to_string(priority);
	output += "\n";
	//cout << "priority:" << priority << endl;
	output += "ppid : ";
	output += to_string(ppid);
	output += "\n";
	//cout << "ppid:" << ppid << endl;
	output += "cpid : ";
	//cout << "cpid:";
	for (unsigned int i = 0; i < cpid.size(); i++)
	{
		output += to_string(cpid[i]);
		output += "    ";
		//cout << cpid[i] << "	";
	}
	output += "\n";
	output += "bufferIO : ";
	output += bufferIO;
	qDebug() << "pcb knows that bufferIO is " << QString::fromStdString(bufferIO);
	output += "\n";
	//cout << "\nbufferIO:" << bufferIO << endl;
	return output;
}

/**************************************************************************************************/
/******************************************PROCESS.cpp内容****************************************/
/**************************************************************************************************/

//根据pid返回其对应PCB
PCB* PROCESS::findPCB(int pid)
{
	//if (pid == -1)return;
	auto iter = process.find(pid);
	if (iter == process.end())
	{
		qDebug() << "pid = " << pid << " does not exists！" << endl;
		//exit(0);
	}
	return &(iter->second);
}

//展示运行、阻塞、就绪队列中进程pid
string PROCESS::showPCBQueue()
{
	string str = "*****************Current Running Process:*******************\n";
	str += "[pid " + to_string(pidRunning) + "] \n";
	int tmp = pidRunning;
	//if (flagPid != -1) tmp = flagPid;
	if (tmp != -1) {
		str += "[process->IR:" + findPCB(tmp)->IR + "] \n";
		str += "[process->timeLast:" + to_string(findPCB(tmp)->timeLast) + "] \n";
		str += "[process->Priority:" + to_string(findPCB(tmp)->priority) + " ] \n";
	}

	str += "\n**********************Block Queue************************\n";
	for (unsigned int i = 0; i < QueueBlock.getQueueNum(); i++)
		str += "[pid " + to_string(QueueBlock.getPID(i)) + "]  ";
	str += '\n';
	if (isMLFQ)
	{
		str += "**********************Ready Queue 0 ************************\n";
		for (unsigned int i = 0; i < MLFQueue.queue0.getQueueNum(); i++)
			str += "[pid " + to_string(MLFQueue.queue0.getPID(i)) + "]  ";
		str += "\n**********************Ready Queue 1 ************************\n";
		for (unsigned int i = 0; i < MLFQueue.queue1.getQueueNum(); i++)
			str += "[pid " + to_string(MLFQueue.queue1.getPID(i)) + "]  ";
		str += "\n**********************Ready Queue 2 ************************\n";
		for (unsigned int i = 0; i < MLFQueue.queue2.getQueueNum(); i++)
			str += "[pid " + to_string(MLFQueue.queue2.getPID(i)) + "]  ";
	}
	else
		str += "**********************Ready Queue ************************\n";
	for (unsigned int i = 0; i < QueueReady.getQueueNum(); i++) {
		str += "[pid " + to_string(QueueReady.getPID(i)) + "] +";
		str += "[process->timeLast:" + to_string(findPCB(QueueReady.getPID(i))->timeLast) + "] \n";
	}
	return str;
}

/*
//打印所有PCB的信息
void PROCESS::outPCBqueue()
{
	auto iter = process.begin();
	for (iter = process.begin(); iter != process.end(); iter++)
	{
		iter->second.outPCB();
		cout << endl;
	}
}

//展示就绪队列
void PROCESS::showReadyQueue()
{
	if (isMLFQ)
	{
		cout << "队列0中的进程展示如下：" << endl;
		for (unsigned int i = 0; i < MLFQueue.queue0.getQueueNum(); i++)
			findPCB(MLFQueue.queue0.queue[i])->outPCB();
		cout << "队列1中的进程展示如下：" << endl;
		for (unsigned int i = 0; i < MLFQueue.queue1.getQueueNum(); i++)
			findPCB(MLFQueue.queue1.queue[i])->outPCB();
		cout << "队列2中的进程展示如下：" << endl;
		for (unsigned int i = 0; i < MLFQueue.queue2.getQueueNum(); i++)
			findPCB(MLFQueue.queue2.queue[i])->outPCB();
	}
	else
		for (unsigned int i = 0; i < QueueReady.getQueueNum(); i++)
			findPCB(QueueReady.queue[i])->outPCB();
}

//展示阻塞队列
void PROCESS::showBlockQueue()
{
	for (unsigned int i = 0; i < QueueBlock.getQueueNum(); i++)
		findPCB(QueueBlock.queue[i])->outPCB();
}
*/

//分析字符串指令，得到枚举类型
Instruct PROCESS::getInstruction(string str)
{
	if (str.find("Computing") != -1)
		return Computing;
	else if (str.find("Fork") != -1)
		return Fork;
	else if (str.find("NewFile") != -1)
		return NewFile;
	else if (str.find("readFile") != -1)
		return readFile;
	else if (str.find("writeFile") != -1)
		return writeFile;
	else if (str.find("DelFile") != -1)
		return DelFile;
	else if (str.find("Access") != -1)
		return Access;
	else if (str.find("Keyboard") != -1)
		return Keyboard;
	else if (str.find("Monitor") != -1)
		return Monitor;
	else if (str.find("Printer") != -1)
		return Printer;
	else if (str.find("END") != -1)
		return END;
}

/*
//创建进程
void PROCESS::createProcess()
{
	int pid = pidMax;
	pidMax++;
	srand(time(NULL));
	int size = rand() % (2 * pageSize) + pageSize;
	process[pid] = PCB(pid, size);
	////********* Memory ************ 申请内存，申请内存成功返回0
	int res = Memory.pageAlloc(pid, size);
	if (res == 0)
		NewToReady(pid);
	else
		cout << "内存不足，创建进程失败！ pid = " << pid;
}
*/

//创建进程，成功返回0，失败返回-1
int PROCESS::createProcess(int priority, string fileName)
{
	int pid = pidMax;
	pidMax++;
	srand(time(NULL));
	int size = rand() % (1 * PAGESIZE) + PAGESIZE;
	process[pid] = PCB(pid, size, priority);
	//* ******** Memory ************ 申请内存，申请内存成功返回0//√
	int res = 0;

	res = myMemoryManager.pageAlloc(pid, size);
	if (res == 0)
		NewToReady(pid);
	else
	{
		qDebug() << "内存不足，创建进程失败！ pid = " << pid;
		pidMax--;
	}
	findPCB(pid)->nextFile = fileName;
	return res;
}

//终止进程
void PROCESS::killProcess(int pid)
{
	PCB* pcb = findPCB(pid);
	for (int i = 0; i < pcb->cpid.size(); i++)
		killProcess(pcb->cpid[i]);  //终止其所有子进程
	if (pcb->ppid != -1)
	{
		PCB* ppcb = findPCB(pcb->ppid); //修改其父进程的子进程序列
		for (int i = 0; i < ppcb->cpid.size(); i++)
		{
			if (ppcb->cpid[i] == pid)
			{
				ppcb->cpid.erase(ppcb->cpid.begin() + i);
				break;
			}
		}
	}
	/*auto iter = process.find(pid);
	process.erase(iter);*/
	if (pcb->state == Ready)
	{
		if (isMLFQ)
			MLFQueue.popProcess(pid, pcb->priority);
		else
			QueueReady.popProcess(pid);
		pcb->state = Terminated;
		qDebug() << "kill pid " << QString::number(pid) << " at Ready !!";
	}
	else if (pcb->state == Block)
	{
		QueueBlock.popProcess(pid);
		pcb->state = Terminated;
	}
	else if (pidRunning == pid)  //如果kill的是正在运行的进程
	{
		pidRunning = -1;
		pcb->state = Terminated;
		//scheduling();   //CPU调度//zmy:注释掉看看
		qDebug() << "kill pid " << QString::number(pid) << " at Running ";
	}
	myMemoryManager.pageFree(pid);//zmy:释放内存//zmy:注释掉可以用来测试内存不足的情况
}

//参数：父进程pid
void PROCESS::fork(int pid)
{
	//新建子进程，PCB同父进程，内存独立。
	//修正ppid和cpid
	//子进程运行，父进程进入就绪态
	PCB* ppcb = findPCB(pid);  //父进程PCB
	int cpid = pidMax;
	if (createProcess(ppcb->priority, ppcb->nextFile) == 0)  //成功创建子进程
	{
		PCB* cpcb = findPCB(cpid); //子进程PCB
		cpcb->size = ppcb->size;
		cpcb->PC = ppcb->PC;
		cpcb->timeLast = ppcb->timeLast - 2;
		cpcb->timeLUCPU = ppcb->timeLUCPU + 2;
		cpcb->bufferIO = ppcb->bufferIO;
		//修正ppid和cpid
		ppcb->cpid.push_back(cpid);
		cpcb->ppid = pid;
		RunningToReady(pid);  //父进程进入就绪态
		//子进程也要打开父进程的文件
		//搞一个for循环，把父进程的内存copy到子进程内存中
		int count = cpcb->size / PAGESIZE + 1;
		string str;
		for (int i = 0; i < count; i++)
		{
			/************* Memory ****************/
			//str += myMemoryManager.pageAccess(pid, PAGESIZE * i, PAGESIZE * (i + 1));
			//str += myMemoryManager.pageAccess(pid, 0, );
		}
		/************* Memory ****************/
		//写内存函数(cpid, str);
		myFileSystem.readFile(ppcb->nextFile, cpid);
		//myMemoryManager.writeVirtual(pid, str);
		ReadyToRunning(cpid);
	}
}
/*
//一直提示无法解析的外部符号……
void Delay_MSec(unsigned int msec)
{
	QTime _Timer = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < _Timer) {
		QApplication::processEvents(QEventLoop::AllEvents);
	}
}
*/

//整体运行Process，应用于读写文件（新建、删除还未完成）
void PROCESS::runProcessAll(int pid)
{
	PCB* pcb = findPCB(pid);
	double timeslice = 0;
	if (isMLFQ && pcb->priority == 0)
		timeslice = timeslice0;
	else if (isMLFQ && pcb->priority == 1)
		timeslice = timeslice1;

	//zmy:先获取IR
	pcb->IR = myMemoryManager.pageAccess(pid, pcb->PC, pcb->PC + INS_LEN);
	pcb->PC += INS_LEN;
	//qDebug() << "Process go to PC:" << QString::number(pcb->PC) << "and it+32 to find IR";

	//zmy:对IR处理去掉填充的空格
	pcb->IR.erase(pcb->IR.find_last_not_of("\n") + 1);
	if (!pcb->IR.empty()) { pcb->IR.erase(pcb->IR.find_last_not_of(" ") + 1); }
	qDebug() << "Process conduct IR:" << QString::fromStdString(pcb->IR) << endl;

	while (pcb->IR != "END")    //每次执行完一条指令，判断是否触发时间片中断
	{
		switch (getInstruction(pcb->IR))
		{
		case NewFile:
		{
			qDebug() << "case NewFile";
			/********* File ******/
			string fileName;
			if (pcb->IR.length() == 7)
				fileName = pcb->nextFile;
			else
				fileName = pcb->IR.substr(8, pcb->IR.size());
			qDebug() << "create New File,fileName = " << QString::fromStdString(fileName);
			myFileSystem.creatFile(fileName);
			qDebug() << "create New File,fileName = " << QString::fromStdString(fileName);
			break;
		}
		case readFile:
		{
			string fileName;
			if (pcb->IR.length() == 8) {
				qDebug() << "Process conduct readFile,use " << QString::fromStdString(pcb->nextFile) << " to read";
				fileName = pcb->nextFile;
			}
			else
				fileName = pcb->IR.erase(0, 9);
			int start = -1, end = -1;
			for (unsigned int i = 0; i < pcb->fileP.size(); i++)  //检查该文件是否已经打开过
			{
				if (pcb->fileP[i].filename == fileName)
				{
					start = pcb->fileP[i].start;
					end = pcb->fileP[i].end;
					qDebug() << "NOT First to open,get start,end : " << QString::number(start) << QString::number(end);
				}
			}
			if (start == -1)  //第一次打开该文件，需要向文件系统要文件
			{
				/******* File ********/
				FL fl = myFileSystem.readFile(fileName, pid);
				pcb->fileP.push_back(fl);
				start = pcb->fileP.back().start;
				end = pcb->fileP.back().end;
				qDebug() << "First to open,ask fileSystem,get start,end : " << QString::number(start) << QString::number(end);
			}
			/******* Memory ********/
			if (start == end) { pcb->bufferIO = ""; }//空文件判断
			else { pcb->bufferIO = myMemoryManager.pageAccess(pid, start, end + 1); }//zmy:end+1
			qDebug() << "Process set its bufferIO as:" << QString::fromStdString(pcb->bufferIO);
			break;
		}
		case writeFile:
		{
			string fileName;
			if (pcb->IR.length() == 9)
				fileName = pcb->nextFile;
			else
				fileName = pcb->IR.erase(0, 10);
			/******* File ********/
			myFileSystem.writeFile(fileName, pcb->bufferIO, 0, pid);
			break;
		}
		case DelFile:
		{
			/******* File ********/
			string fileName;
			if (!pcb->IR.empty()) { pcb->IR.erase(pcb->IR.find_last_not_of(" ") + 1); }

			if (pcb->IR.length() == 7)
				fileName = pcb->nextFile;
			else
				fileName = pcb->IR.erase(0, 8);
			myFileSystem.deleteFile(fileName);
			break;
		}
		default:
			qDebug() << "ERROR:can't resolve IR";
			qDebug() << QString::fromStdString(pcb->IR) << endl;
		}
		if (myInterrupt->interruptTimeSlice(timeslice, pcb))
			break;

		//再次获取指令
		pcb->IR = myMemoryManager.pageAccess(pid, pcb->PC, pcb->PC + INS_LEN);
		pcb->PC += INS_LEN;
		qDebug() << "Process go to PC:" << QString::number(pcb->PC) << "and it+32 to find IR";

		//zmy:对IR处理去掉填充的空格
		pcb->IR.erase(pcb->IR.find_last_not_of("\n") + 1);
		if (!pcb->IR.empty()) { pcb->IR.erase(pcb->IR.find_last_not_of(" ") + 1); }
		qDebug() << "Process conduct IR:" << QString::fromStdString(pcb->IR) << endl;
	}
	if (pcb->IR == "END")
		killProcess(pid);
	return;
}

//逐步运行Process，在runFile时使用
//还有下一条指令返回0，结束返回1，请求设备返回2，
int PROCESS::runProcessStep(int pid)
{
	PCB* pcb = findPCB(pid);
	string tmp;
	double timeslice = 0;
	//time_t timep;
	//time(&timep);
	//pcb->timeLUCPU = timep;
	if (isMLFQ && pcb->priority == 0)
		timeslice = timeslice0;
	else if (isMLFQ && pcb->priority == 1)
		timeslice = timeslice1;
	//dxy:先判断时间片中断
	if (myInterrupt->interruptTimeSlice(timeslice, myPS.findPCB(myPS.pidRunning))) return -1;

	//zmy:先获取IR并处理空格
	pcb->IR = myMemoryManager.pageAccess(pid, pcb->PC, pcb->PC + INS_LEN);
	pcb->PC += INS_LEN;
	//qDebug() << "Process go to PC:" << QString::number(pcb->PC) << "and it+32 to find IR";
	pcb->IR.erase(pcb->IR.find_last_not_of("\n") + 1);
	if (!pcb->IR.empty()) { pcb->IR.erase(pcb->IR.find_last_not_of(" ") + 1); }
	qDebug() << "Process conduct IR:" << QString::fromStdString(pcb->IR) << endl;

	//如果end
	if (pcb->IR == "END") { killProcess(pid); return 1; }

	//不是end：	
	switch (getInstruction(pcb->IR))
	{
	case Computing:
	{
		//获取时间
		//string time = pcb->IR.erase(0, 10);
		string time = pcb->IR.substr(11, pcb->IR.size());
		//Sleep(atoi(time.c_str()));
		//QThread::msleep(5000);//zmy:试试
		//Delay_MSec(5000);//zmy:try2
		qDebug() << "computed!";
		break;
	}
	case Fork:
		fork(pid);
		break;
	case NewFile:
	{
		/********* File ******/
		string fileName;
		if (pcb->IR.length() == 7)
			fileName = pcb->nextFile;
		else
			fileName = pcb->IR.substr(8, pcb->IR.size());

		myFileSystem.creatFile(fileName);
		qDebug() << "create New File,fileName = " << QString::fromStdString(fileName);
		break;
	}
	case readFile:
	{
		string fileName;
		if (pcb->IR.length() == 8) {
			qDebug() << "Process conduct readFile,use " << QString::fromStdString(pcb->nextFile) << " to read";
			fileName = pcb->nextFile;
		}
		else
			fileName = pcb->IR.substr(9, pcb->IR.size());

		qDebug() << "readFile Filename is: " << QString::fromStdString(fileName);

		int start = -1, end = -1;
		for (unsigned int i = 0; i < pcb->fileP.size(); i++)  //检查该文件是否已经打开过
		{
			if (pcb->fileP[i].filename == fileName)
			{
				start = pcb->fileP[i].start;
				end = pcb->fileP[i].end;
				qDebug() << "NOT First to open,get start,end : " << QString::number(start) << QString::number(end);
			}
		}
		if (start == -1)  //第一次打开该文件，需要向文件系统要文件
		{
			/******* File ********/
			FL fl = myFileSystem.readFile(fileName, pid);
			pcb->fileP.push_back(fl);
			start = pcb->fileP.back().start;
			end = pcb->fileP.back().end;
			qDebug() << "First to open,ask fileSystem,get start,end : " << QString::number(start) << QString::number(end);
		}
		/******* Memory ********/
		if (start == end) { pcb->bufferIO = ""; }//空文件判断
		else { pcb->bufferIO = myMemoryManager.pageAccess(pid, start, end + 1); }//zmy:end+1
		qDebug() << "Process set its bufferIO as:" << QString::fromStdString(pcb->bufferIO);
		break;
	}
	case writeFile:
	{
		string fileName;
		if (pcb->IR.length() == 9)
			fileName = pcb->nextFile;
		else
			fileName = pcb->IR.substr(10, pcb->IR.size());
		/******* File ********/
		myFileSystem.writeFile(fileName, pcb->bufferIO, 0, pid);
		break;
	}
	case DelFile:
	{
		/******* File ********/
		string fileName;
		//if (!pcb->IR.empty()) { pcb->IR.erase(pcb->IR.find_last_not_of(" ") + 1); }

		if (pcb->IR.length() == 7)
			fileName = pcb->nextFile;
		else
			fileName = pcb->IR.substr(8, pcb->IR.size());;
		myFileSystem.deleteFile(fileName);
		break;
	}
	case Access:
	{
		//获取地址
		string address = pcb->IR.substr(8, pcb->IR.size());;//zmy:应该是7
		/*********************** Memory *******************/
		int start = atoi(address.c_str()), end = (atoi(address.c_str()));
		if (myMemoryManager.checkPhysical(pid, start, end) == 0); //未发生缺页中断
			//pcb->bufferIO = myMemoryManager.pageAccess(pid, start, end + 1);
		else  //发生缺页中断
			myInterrupt->interruptPageMiss(pid, start, end);
		qDebug() << "accessd!";
		break;
	}
	case Keyboard:
		myInterrupt->interruptIO(pid, "Keyboard");
		return -1;
	case Monitor:
		myInterrupt->interruptIO(pid, "Monitor");
		return -1;
	case Printer:
		myInterrupt->interruptIO(pid, "printer");
		return -1;
	default:
		qDebug() << "ERROR:can't resolve IR";
		qDebug() << QString::fromStdString(pcb->IR) << endl;
		//cout << "无法解析指令" << pcb->IR << "!" << endl;
		//exit(0);
	}
	pcb->timeLast -= 2;
	//zmy:这里没有看懂
	//time(&timep);
	//pcb->timeLUCPU = timep;
	//qDebug()<<"time is "<< timep

	return 0;
}


//first come first served
int PROCESS::FCFS(QUEUE QueueReady)
{
	qDebug() << "*********FCFS********";
	int num = QueueReady.getQueueNum();
	if (num == 0)  //就绪队列为空
	{
		return -1;
		//qDebug() << "no process running now";
		//cout << "当前系统无进程可运行！" << endl;

	}
	else  //取队列头的进程运行
	{
		//this->ReadyToRunning(pidRunning);
		return  QueueReady.getPID(0);
	}
}

//返回需要CPU时间最短的进程pid
int PROCESS::findShortestJob(vector<int> queue)
{
	if (queue.size() == 0)
	{
		qDebug() << "就绪队列为空！在不该调用函数findShortest时进行调用，请检查代码逻辑！" << endl;
		//exit(0);
		return -1;
	}
	int minPid = queue[0], minTime = findPCB(queue[0])->timeLast;
	for (int i = 1; i < queue.size(); i++)
	{
		if (minTime > findPCB(queue[i])->timeLast)
		{
			minTime = findPCB(queue[i])->timeLast;
			minPid = queue[i];
		}
		else if (minTime == findPCB(queue[i])->timeLast)  //比较优先级
		{
			if (findPCB(minPid)->priority > findPCB(queue[i])->priority)
			{
				minTime = findPCB(queue[i])->timeLast;
				minPid = queue[i];
			}
		}
	}
	qDebug() << "find shortest job choose pid :" << QString::number(minPid);
	return minPid;
}

//shorest job first
int PROCESS::SJF()
{
	qDebug() << "*********SJF********";
	int num = QueueReady.getQueueNum();
	if (num == 0)
	{
		return -1;
		cout << "当前系统无进程可运行！" << endl;
	}
	else
	{
		//ReadyToRunning(pidRunning);

		return findShortestJob(QueueReady.queue);
	}
}

//SJF==1 && preemptive==1 && 有进程进入就绪态时使用，判断是否抢占
int PROCESS::preeSJF()
{
	//找到就绪队列中时间最短的进程
	int pidNew = findShortestJob(QueueReady.queue);
	PCB* pcbNew = findPCB(pidNew);
	int timeNew = pcbNew->timeLast;
	//获得runningPid的时间
	if (pidRunning == -1)return pidNew;//zmy:0602发现这里要处理为-1的情况
	PCB* pcbRunning = findPCB(pidRunning);
	time_t timep;
	time(&timep);
	int timeRunning = pcbRunning->timeLast; //- difftime(timep, pcbRunning->timeLUCPU);
	if (timeNew < timeRunning)  //抢占CPU
	{
		RunningToReady(pidRunning);
		qDebug() << "pidRunning" << pidRunning << "RunningToReady";
		ReadyToRunning(pidNew);
		qDebug() << "pidNew" << pidNew << "ReadyToRunning";
		return pidNew;
	}
	return -1;
}

//multilevel feedback queue
int PROCESS::MLFQ()
{
	int num0 = MLFQueue.getQueueNum(0);   //队列0中的进程数量
	if (num0 > 0)  //队列0采用RR算法，因此取队列头的进程运行
	{
		int pid = MLFQueue.getPID(0, 0);  //被调度的进程pid
		//ReadyToRunning(pidRunning);
		return pid;
	}
	else
	{
		int num1 = MLFQueue.getQueueNum(1);   //队列1中的进程数量
		if (num1 > 0)  //队列1采用RR算法，因此取队列头的进程运行
		{
			int pid = MLFQueue.getPID(1, 0);  //被调度的进程pid
			//ReadyToRunning(pidRunning);
			return pid;
		}
		else  //队列2采用FCFS调度算法
		{
			return FCFS(MLFQueue.queue2);
		}
	}
}

//CPU调度,返回下一个要运行的pid，没有返回-1
int PROCESS::scheduling()
{
	int pidToRun;
	//选取调度算法进行调度
	if (isFCFS)
		pidToRun = FCFS(QueueReady);
	else if (isSJF)
		pidToRun = SJF();
	else if (isMLFQ)
		pidToRun = MLFQ();
	else
	{
		cout << "未选择调度算法，系统无法正常运行，请重进系统！" << endl;
		exit(0);
		pidToRun = -1;
	}
	if (pidToRun != -1) { ReadyToRunning(pidToRun); }
	pidRunning = pidToRun;
	qDebug() << "schedual choose pidRunning =" << pidRunning;
	return pidToRun;
}

//进程创建成功，进入就绪态
void PROCESS::NewToReady(int pid)
{
	PCB* pcb = findPCB(pid);
	pcb->state = Ready;
	if (isMLFQ)
		MLFQueue.addProcess(pid, pcb->priority);
	else
	{
		QueueReady.addProcess(pid);

	}
	qDebug() << "pid:" << pid << "New To Ready!";
}

//进程被调度，进入运行态

void PROCESS::ReadyToRunning(int pid)
{
	qDebug() << "pid" << QString::number(pid) << "ready to running";
	PCB* pcb = findPCB(pid);
	pcb->state = Running;
	pidRunning = pid;
	if (isMLFQ)
		MLFQueue.popProcess(pid, pcb->priority);
	else
		QueueReady.popProcess(pid);

	qDebug() << "pid " << QString::number(pid) << "Ready to Running,disappear from ready queue!";

	//runProcess(pidRunning);
}


//运行态 ——> 就绪态
void PROCESS::RunningToReady(int pid)
{
	PCB* pcb = findPCB(pid);
	pidRunning = -1;
	pcb->state = Ready;//改变状态
	if (isMLFQ)  //如果该进程优先级==0||1，则降一级
	{
		if (pcb->priority == 0 || pcb->priority == 1)
			pcb->priority++;
		MLFQueue.addProcess(pid, pcb->priority);
	}
	else
		QueueReady.addProcess(pid);//添加到ready队列
	//修改程序剩余执行时间
	time_t timep;
	time(&timep);
	//pcb->timeLast = difftime(timep, pcb->timeLUCPU);
}

//运行态 ——> 阻塞态
void PROCESS::RunningToBlock(int pid)
{
	PCB* pcb = findPCB(pid);
	qDebug() << "PROCESS::RunningToBlock:findPCB" << pid;
	pidRunning = -1;
	pcb->state = Block;
	if (isMLFQ)  //如果该进程优先级==0||1，则降一级
		if (pcb->priority == 0 || pcb->priority == 1)
			pcb->priority++;
	QueueBlock.addProcess(pid);
	//修改程序剩余执行时间
	time_t timep;
	time(&timep);
	//pcb->timeLast = difftime(timep, pcb->timeLUCPU);
}


//阻塞态 ——> 就绪态
void PROCESS::BlockToReady(int pid)
{
	PCB* pcb = findPCB(pid);
	if (pcb->state != Terminated)   //进程在使用IO设备过程中有可能已经被kill，则QueueBlock中不存在该进程
	{
		QueueBlock.popProcess(pid);
		pcb->state = Ready;
		if (isMLFQ)
			MLFQueue.addProcess(pid, findPCB(pid)->priority);
		else
		{
			QueueReady.addProcess(pid);
			qDebug() << "PROCESS:BlockToReady:pid" << pid;
			if (isSJF && isPreemptive)  //如果CPU调度算法为抢占式SJF
				preeSJF();  //每当有进程进入就绪队列时，检查是否需要抢占
		}
	}
}

/**************************************************************************************************/
/******************************************QUEUE.cpp内容****************************************/
/**************************************************************************************************/

int QUEUE::getQueueNum() //返回队列中的进程数量
{
	return queue.size();
}
int QUEUE::getPID(int pos) //根据进程处在队列中位置(从0始)返回pid
{
	return queue[pos];
}

//添加进程
void QUEUE::addProcess(int pid)
{
	queue.push_back(pid);
}

//删除进程
void QUEUE::popProcess(int pid)
{
	int i = 0;
	for (i = 0; i < queue.size(); i++)
	{
		if (pid == queue[i])
		{
			queue.erase(queue.begin() + i);
			return;
		}
	}
	if (i == queue.size())
	{
		//qDebug() << QString::number(pid) << "does not exists.";
		//cout << "你要删除的进程pid = " << pid << " 不存在！" << endl;
		//exit(0);
	}
}
