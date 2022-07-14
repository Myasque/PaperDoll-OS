#include"PROCESS.h"
#define PAGESIZE 1024
int isMLFQ = 0;  //�Ƿ�ѡ��MLFQ�㷨����=1����=0
int isSJF = 0;  //�Ƿ�ѡ��SJF�㷨����=1����=0
int isFCFS = 0;  //�Ƿ�ѡ��FCFS�㷨����=1����=0
int isPreemptive = 0;;  //�Ƿ���ռ����=1����=0����SJFģʽ��ʹ��
double timeslice0 = 5;   //����0��ʱ��Ƭ
double timeslice1 = 3;  //����1��ʱ��Ƭ
//extern const int pageSize ;  //ҳ���С
#include <QThread>
extern FileSystem myFileSystem;//�ļ�ϵͳ
extern memoryManager myMemoryManager;//�ڴ����
extern DeviceSystem myDeviceSystem;
extern PROCESS myPS;

/**************************************************************************************************/
/******************************************INTERRUPT.cpp����***************************************/
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
		cout << "������ж�����:" << type << "!" << endl;
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
		if (difftime(temp, pcb->timeLUCPU) >= timeslice) //�������еĽ���ʱ��Ƭ�þ�
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
	qDebug() << "�����ֶ������жϣ�" << endl;
	//<< "��ǰ�������еĽ�����ϢΪ��" << endl;
//pcb->outPCB();
}

/**************************************************************************************************/
/******************************************MLFQUEUE.cpp����****************************************/
/**************************************************************************************************/

int MLFQUEUE::getQueueNum(int num)  //���ض����еĽ�������������Ϊ���б��(ȡֵ0/1/2)
{
	if (num == 0)
		return queue0.getQueueNum();
	else if (num == 1)
		return queue1.getQueueNum();
	else if (num == 2)
		return queue2.getQueueNum();
}

int MLFQUEUE::getPID(int queueNum, int pos)  //���ݽ������ڶ��кźʹ��ڶ�����λ��(��0ʼ)����pid
{
	if (queueNum == 0)
		return queue0.getPID(pos);
	else if (queueNum == 1)
		return queue1.getPID(pos);
	else if (queueNum == 2)
		return queue2.getPID(pos);
}

//���ݽ������ȼ�����������ӵ�queue0 || queue1 || queue2
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
		qDebug() << "��⵽�Ƿ��Ľ������ȼ����� pid=" << pid << "	priority=" << priority << endl;
		//exit(0);
	}
}

//���ݽ������ȼ��������̴�queue0 || queue1 || queue2��ɾ��
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
		qDebug() << "illegal priority���� pid=" << pid << "	priority=" << priority << endl;
		//exit(0);
	}
}

/**************************************************************************************************/
/******************************************PCB.cpp����****************************************/
/**************************************************************************************************/
PCB::PCB()
{
	this->pid = 12345;
	this->ppid = -1;//zmy:��ʼ��ppid
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
	this->ppid = -1;//zmy:��ʼ��ppid
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
	this->ppid = -1;//zmy:��ʼ��ppid
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

//���ڽ��̵�״̬������ö�ٲ��ܴ�ӡ�ַ����������øú�����ӡ״̬
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
		cout << "����" << this->pid << "״̬�����쳣��" << this->state << endl;
		//exit(0);
	}
}

//��PCB��������Ϣ���
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
/******************************************PROCESS.cpp����****************************************/
/**************************************************************************************************/

//����pid�������ӦPCB
PCB* PROCESS::findPCB(int pid)
{
	//if (pid == -1)return;
	auto iter = process.find(pid);
	if (iter == process.end())
	{
		qDebug() << "pid = " << pid << " does not exists��" << endl;
		//exit(0);
	}
	return &(iter->second);
}

//չʾ���С����������������н���pid
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
//��ӡ����PCB����Ϣ
void PROCESS::outPCBqueue()
{
	auto iter = process.begin();
	for (iter = process.begin(); iter != process.end(); iter++)
	{
		iter->second.outPCB();
		cout << endl;
	}
}

//չʾ��������
void PROCESS::showReadyQueue()
{
	if (isMLFQ)
	{
		cout << "����0�еĽ���չʾ���£�" << endl;
		for (unsigned int i = 0; i < MLFQueue.queue0.getQueueNum(); i++)
			findPCB(MLFQueue.queue0.queue[i])->outPCB();
		cout << "����1�еĽ���չʾ���£�" << endl;
		for (unsigned int i = 0; i < MLFQueue.queue1.getQueueNum(); i++)
			findPCB(MLFQueue.queue1.queue[i])->outPCB();
		cout << "����2�еĽ���չʾ���£�" << endl;
		for (unsigned int i = 0; i < MLFQueue.queue2.getQueueNum(); i++)
			findPCB(MLFQueue.queue2.queue[i])->outPCB();
	}
	else
		for (unsigned int i = 0; i < QueueReady.getQueueNum(); i++)
			findPCB(QueueReady.queue[i])->outPCB();
}

//չʾ��������
void PROCESS::showBlockQueue()
{
	for (unsigned int i = 0; i < QueueBlock.getQueueNum(); i++)
		findPCB(QueueBlock.queue[i])->outPCB();
}
*/

//�����ַ���ָ��õ�ö������
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
//��������
void PROCESS::createProcess()
{
	int pid = pidMax;
	pidMax++;
	srand(time(NULL));
	int size = rand() % (2 * pageSize) + pageSize;
	process[pid] = PCB(pid, size);
	////********* Memory ************ �����ڴ棬�����ڴ�ɹ�����0
	int res = Memory.pageAlloc(pid, size);
	if (res == 0)
		NewToReady(pid);
	else
		cout << "�ڴ治�㣬��������ʧ�ܣ� pid = " << pid;
}
*/

//�������̣��ɹ�����0��ʧ�ܷ���-1
int PROCESS::createProcess(int priority, string fileName)
{
	int pid = pidMax;
	pidMax++;
	srand(time(NULL));
	int size = rand() % (1 * PAGESIZE) + PAGESIZE;
	process[pid] = PCB(pid, size, priority);
	//* ******** Memory ************ �����ڴ棬�����ڴ�ɹ�����0//��
	int res = 0;

	res = myMemoryManager.pageAlloc(pid, size);
	if (res == 0)
		NewToReady(pid);
	else
	{
		qDebug() << "�ڴ治�㣬��������ʧ�ܣ� pid = " << pid;
		pidMax--;
	}
	findPCB(pid)->nextFile = fileName;
	return res;
}

//��ֹ����
void PROCESS::killProcess(int pid)
{
	PCB* pcb = findPCB(pid);
	for (int i = 0; i < pcb->cpid.size(); i++)
		killProcess(pcb->cpid[i]);  //��ֹ�������ӽ���
	if (pcb->ppid != -1)
	{
		PCB* ppcb = findPCB(pcb->ppid); //�޸��丸���̵��ӽ�������
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
	else if (pidRunning == pid)  //���kill�����������еĽ���
	{
		pidRunning = -1;
		pcb->state = Terminated;
		//scheduling();   //CPU����//zmy:ע�͵�����
		qDebug() << "kill pid " << QString::number(pid) << " at Running ";
	}
	myMemoryManager.pageFree(pid);//zmy:�ͷ��ڴ�//zmy:ע�͵��������������ڴ治������
}

//������������pid
void PROCESS::fork(int pid)
{
	//�½��ӽ��̣�PCBͬ�����̣��ڴ������
	//����ppid��cpid
	//�ӽ������У������̽������̬
	PCB* ppcb = findPCB(pid);  //������PCB
	int cpid = pidMax;
	if (createProcess(ppcb->priority, ppcb->nextFile) == 0)  //�ɹ������ӽ���
	{
		PCB* cpcb = findPCB(cpid); //�ӽ���PCB
		cpcb->size = ppcb->size;
		cpcb->PC = ppcb->PC;
		cpcb->timeLast = ppcb->timeLast - 2;
		cpcb->timeLUCPU = ppcb->timeLUCPU + 2;
		cpcb->bufferIO = ppcb->bufferIO;
		//����ppid��cpid
		ppcb->cpid.push_back(cpid);
		cpcb->ppid = pid;
		RunningToReady(pid);  //�����̽������̬
		//�ӽ���ҲҪ�򿪸����̵��ļ�
		//��һ��forѭ�����Ѹ����̵��ڴ�copy���ӽ����ڴ���
		int count = cpcb->size / PAGESIZE + 1;
		string str;
		for (int i = 0; i < count; i++)
		{
			/************* Memory ****************/
			//str += myMemoryManager.pageAccess(pid, PAGESIZE * i, PAGESIZE * (i + 1));
			//str += myMemoryManager.pageAccess(pid, 0, );
		}
		/************* Memory ****************/
		//д�ڴ溯��(cpid, str);
		myFileSystem.readFile(ppcb->nextFile, cpid);
		//myMemoryManager.writeVirtual(pid, str);
		ReadyToRunning(cpid);
	}
}
/*
//һֱ��ʾ�޷��������ⲿ���š���
void Delay_MSec(unsigned int msec)
{
	QTime _Timer = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < _Timer) {
		QApplication::processEvents(QEventLoop::AllEvents);
	}
}
*/

//��������Process��Ӧ���ڶ�д�ļ����½���ɾ����δ��ɣ�
void PROCESS::runProcessAll(int pid)
{
	PCB* pcb = findPCB(pid);
	double timeslice = 0;
	if (isMLFQ && pcb->priority == 0)
		timeslice = timeslice0;
	else if (isMLFQ && pcb->priority == 1)
		timeslice = timeslice1;

	//zmy:�Ȼ�ȡIR
	pcb->IR = myMemoryManager.pageAccess(pid, pcb->PC, pcb->PC + INS_LEN);
	pcb->PC += INS_LEN;
	//qDebug() << "Process go to PC:" << QString::number(pcb->PC) << "and it+32 to find IR";

	//zmy:��IR����ȥ�����Ŀո�
	pcb->IR.erase(pcb->IR.find_last_not_of("\n") + 1);
	if (!pcb->IR.empty()) { pcb->IR.erase(pcb->IR.find_last_not_of(" ") + 1); }
	qDebug() << "Process conduct IR:" << QString::fromStdString(pcb->IR) << endl;

	while (pcb->IR != "END")    //ÿ��ִ����һ��ָ��ж��Ƿ񴥷�ʱ��Ƭ�ж�
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
			for (unsigned int i = 0; i < pcb->fileP.size(); i++)  //�����ļ��Ƿ��Ѿ��򿪹�
			{
				if (pcb->fileP[i].filename == fileName)
				{
					start = pcb->fileP[i].start;
					end = pcb->fileP[i].end;
					qDebug() << "NOT First to open,get start,end : " << QString::number(start) << QString::number(end);
				}
			}
			if (start == -1)  //��һ�δ򿪸��ļ�����Ҫ���ļ�ϵͳҪ�ļ�
			{
				/******* File ********/
				FL fl = myFileSystem.readFile(fileName, pid);
				pcb->fileP.push_back(fl);
				start = pcb->fileP.back().start;
				end = pcb->fileP.back().end;
				qDebug() << "First to open,ask fileSystem,get start,end : " << QString::number(start) << QString::number(end);
			}
			/******* Memory ********/
			if (start == end) { pcb->bufferIO = ""; }//���ļ��ж�
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

		//�ٴλ�ȡָ��
		pcb->IR = myMemoryManager.pageAccess(pid, pcb->PC, pcb->PC + INS_LEN);
		pcb->PC += INS_LEN;
		qDebug() << "Process go to PC:" << QString::number(pcb->PC) << "and it+32 to find IR";

		//zmy:��IR����ȥ�����Ŀո�
		pcb->IR.erase(pcb->IR.find_last_not_of("\n") + 1);
		if (!pcb->IR.empty()) { pcb->IR.erase(pcb->IR.find_last_not_of(" ") + 1); }
		qDebug() << "Process conduct IR:" << QString::fromStdString(pcb->IR) << endl;
	}
	if (pcb->IR == "END")
		killProcess(pid);
	return;
}

//������Process����runFileʱʹ��
//������һ��ָ���0����������1�������豸����2��
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
	//dxy:���ж�ʱ��Ƭ�ж�
	if (myInterrupt->interruptTimeSlice(timeslice, myPS.findPCB(myPS.pidRunning))) return -1;

	//zmy:�Ȼ�ȡIR������ո�
	pcb->IR = myMemoryManager.pageAccess(pid, pcb->PC, pcb->PC + INS_LEN);
	pcb->PC += INS_LEN;
	//qDebug() << "Process go to PC:" << QString::number(pcb->PC) << "and it+32 to find IR";
	pcb->IR.erase(pcb->IR.find_last_not_of("\n") + 1);
	if (!pcb->IR.empty()) { pcb->IR.erase(pcb->IR.find_last_not_of(" ") + 1); }
	qDebug() << "Process conduct IR:" << QString::fromStdString(pcb->IR) << endl;

	//���end
	if (pcb->IR == "END") { killProcess(pid); return 1; }

	//����end��	
	switch (getInstruction(pcb->IR))
	{
	case Computing:
	{
		//��ȡʱ��
		//string time = pcb->IR.erase(0, 10);
		string time = pcb->IR.substr(11, pcb->IR.size());
		//Sleep(atoi(time.c_str()));
		//QThread::msleep(5000);//zmy:����
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
		for (unsigned int i = 0; i < pcb->fileP.size(); i++)  //�����ļ��Ƿ��Ѿ��򿪹�
		{
			if (pcb->fileP[i].filename == fileName)
			{
				start = pcb->fileP[i].start;
				end = pcb->fileP[i].end;
				qDebug() << "NOT First to open,get start,end : " << QString::number(start) << QString::number(end);
			}
		}
		if (start == -1)  //��һ�δ򿪸��ļ�����Ҫ���ļ�ϵͳҪ�ļ�
		{
			/******* File ********/
			FL fl = myFileSystem.readFile(fileName, pid);
			pcb->fileP.push_back(fl);
			start = pcb->fileP.back().start;
			end = pcb->fileP.back().end;
			qDebug() << "First to open,ask fileSystem,get start,end : " << QString::number(start) << QString::number(end);
		}
		/******* Memory ********/
		if (start == end) { pcb->bufferIO = ""; }//���ļ��ж�
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
		//��ȡ��ַ
		string address = pcb->IR.substr(8, pcb->IR.size());;//zmy:Ӧ����7
		/*********************** Memory *******************/
		int start = atoi(address.c_str()), end = (atoi(address.c_str()));
		if (myMemoryManager.checkPhysical(pid, start, end) == 0); //δ����ȱҳ�ж�
			//pcb->bufferIO = myMemoryManager.pageAccess(pid, start, end + 1);
		else  //����ȱҳ�ж�
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
		//cout << "�޷�����ָ��" << pcb->IR << "!" << endl;
		//exit(0);
	}
	pcb->timeLast -= 2;
	//zmy:����û�п���
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
	if (num == 0)  //��������Ϊ��
	{
		return -1;
		//qDebug() << "no process running now";
		//cout << "��ǰϵͳ�޽��̿����У�" << endl;

	}
	else  //ȡ����ͷ�Ľ�������
	{
		//this->ReadyToRunning(pidRunning);
		return  QueueReady.getPID(0);
	}
}

//������ҪCPUʱ����̵Ľ���pid
int PROCESS::findShortestJob(vector<int> queue)
{
	if (queue.size() == 0)
	{
		qDebug() << "��������Ϊ�գ��ڲ��õ��ú���findShortestʱ���е��ã���������߼���" << endl;
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
		else if (minTime == findPCB(queue[i])->timeLast)  //�Ƚ����ȼ�
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
		cout << "��ǰϵͳ�޽��̿����У�" << endl;
	}
	else
	{
		//ReadyToRunning(pidRunning);

		return findShortestJob(QueueReady.queue);
	}
}

//SJF==1 && preemptive==1 && �н��̽������̬ʱʹ�ã��ж��Ƿ���ռ
int PROCESS::preeSJF()
{
	//�ҵ�����������ʱ����̵Ľ���
	int pidNew = findShortestJob(QueueReady.queue);
	PCB* pcbNew = findPCB(pidNew);
	int timeNew = pcbNew->timeLast;
	//���runningPid��ʱ��
	if (pidRunning == -1)return pidNew;//zmy:0602��������Ҫ����Ϊ-1�����
	PCB* pcbRunning = findPCB(pidRunning);
	time_t timep;
	time(&timep);
	int timeRunning = pcbRunning->timeLast; //- difftime(timep, pcbRunning->timeLUCPU);
	if (timeNew < timeRunning)  //��ռCPU
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
	int num0 = MLFQueue.getQueueNum(0);   //����0�еĽ�������
	if (num0 > 0)  //����0����RR�㷨�����ȡ����ͷ�Ľ�������
	{
		int pid = MLFQueue.getPID(0, 0);  //�����ȵĽ���pid
		//ReadyToRunning(pidRunning);
		return pid;
	}
	else
	{
		int num1 = MLFQueue.getQueueNum(1);   //����1�еĽ�������
		if (num1 > 0)  //����1����RR�㷨�����ȡ����ͷ�Ľ�������
		{
			int pid = MLFQueue.getPID(1, 0);  //�����ȵĽ���pid
			//ReadyToRunning(pidRunning);
			return pid;
		}
		else  //����2����FCFS�����㷨
		{
			return FCFS(MLFQueue.queue2);
		}
	}
}

//CPU����,������һ��Ҫ���е�pid��û�з���-1
int PROCESS::scheduling()
{
	int pidToRun;
	//ѡȡ�����㷨���е���
	if (isFCFS)
		pidToRun = FCFS(QueueReady);
	else if (isSJF)
		pidToRun = SJF();
	else if (isMLFQ)
		pidToRun = MLFQ();
	else
	{
		cout << "δѡ������㷨��ϵͳ�޷��������У����ؽ�ϵͳ��" << endl;
		exit(0);
		pidToRun = -1;
	}
	if (pidToRun != -1) { ReadyToRunning(pidToRun); }
	pidRunning = pidToRun;
	qDebug() << "schedual choose pidRunning =" << pidRunning;
	return pidToRun;
}

//���̴����ɹ����������̬
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

//���̱����ȣ���������̬

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


//����̬ ����> ����̬
void PROCESS::RunningToReady(int pid)
{
	PCB* pcb = findPCB(pid);
	pidRunning = -1;
	pcb->state = Ready;//�ı�״̬
	if (isMLFQ)  //����ý������ȼ�==0||1����һ��
	{
		if (pcb->priority == 0 || pcb->priority == 1)
			pcb->priority++;
		MLFQueue.addProcess(pid, pcb->priority);
	}
	else
		QueueReady.addProcess(pid);//��ӵ�ready����
	//�޸ĳ���ʣ��ִ��ʱ��
	time_t timep;
	time(&timep);
	//pcb->timeLast = difftime(timep, pcb->timeLUCPU);
}

//����̬ ����> ����̬
void PROCESS::RunningToBlock(int pid)
{
	PCB* pcb = findPCB(pid);
	qDebug() << "PROCESS::RunningToBlock:findPCB" << pid;
	pidRunning = -1;
	pcb->state = Block;
	if (isMLFQ)  //����ý������ȼ�==0||1����һ��
		if (pcb->priority == 0 || pcb->priority == 1)
			pcb->priority++;
	QueueBlock.addProcess(pid);
	//�޸ĳ���ʣ��ִ��ʱ��
	time_t timep;
	time(&timep);
	//pcb->timeLast = difftime(timep, pcb->timeLUCPU);
}


//����̬ ����> ����̬
void PROCESS::BlockToReady(int pid)
{
	PCB* pcb = findPCB(pid);
	if (pcb->state != Terminated)   //������ʹ��IO�豸�������п����Ѿ���kill����QueueBlock�в����ڸý���
	{
		QueueBlock.popProcess(pid);
		pcb->state = Ready;
		if (isMLFQ)
			MLFQueue.addProcess(pid, findPCB(pid)->priority);
		else
		{
			QueueReady.addProcess(pid);
			qDebug() << "PROCESS:BlockToReady:pid" << pid;
			if (isSJF && isPreemptive)  //���CPU�����㷨Ϊ��ռʽSJF
				preeSJF();  //ÿ���н��̽����������ʱ������Ƿ���Ҫ��ռ
		}
	}
}

/**************************************************************************************************/
/******************************************QUEUE.cpp����****************************************/
/**************************************************************************************************/

int QUEUE::getQueueNum() //���ض����еĽ�������
{
	return queue.size();
}
int QUEUE::getPID(int pos) //���ݽ��̴��ڶ�����λ��(��0ʼ)����pid
{
	return queue[pos];
}

//��ӽ���
void QUEUE::addProcess(int pid)
{
	queue.push_back(pid);
}

//ɾ������
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
		//cout << "��Ҫɾ���Ľ���pid = " << pid << " �����ڣ�" << endl;
		//exit(0);
	}
}
