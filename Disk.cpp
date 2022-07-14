#define DISK_FCFS 1
#define DISK_SSTF 2
#define DISK_SCAN 3
#define DISK_C_SCAN 4
#define DISK_LOOK 5
#define DISK_C_LOOK 6

#include "disk.h"
#include<math.h> 
//zmy
int diskAlgorithm = 0;

void Disk::initDisk() {
	seekQueueNum = 10;
	trackNum = 200;
	srand(time(NULL));
	curHead = rand() % 199 + 1;
	//disk.output[20]={0};
	outputNum = 0;
	//srand(time(NULL));
	for (int i = 0; i < seekQueueNum; i++) {
		seekQueue[i] = rand() % 199 + 1;//Ëæ»ú²úÉú
		qDebug() << "seekQueue[" << i << "] = " << seekQueue[i];
	}
}
string Disk::showDisk()
{
	string tmp = "";
	tmp += "curHead = ";
	tmp += to_string(curHead);
	tmp += "\n";
	for (int i = 0; i < seekQueueNum; i++) {
		tmp += "seekQueue[" + to_string(i);
		tmp += "] = " + to_string(seekQueue[i]);
		tmp += "\n";
	}
	qDebug() << "showDisk tmp= " << QString::fromStdString(tmp);
	return tmp;
}
void Disk::seekPath(int type) {

	if (type == DISK_FCFS) FCFS();
	if (type == DISK_SSTF) SSTF();
	if (type == DISK_SCAN) SCAN();
	if (type == DISK_C_SCAN) C_SCAN();
	if (type == DISK_LOOK) LOOK();
	if (type == DISK_C_LOOK) C_LOOK();
}

void Disk::FCFS() {
	// int seekQueue[20]={98,183,37,122,119,14,124,65,67,198,105,53};
	// int seekQueueNum=12;
	//initDisk();
	//cout << "FCFS:" << endl;
	int tmpH = curHead;
	while (seekQueueNum > 0) {
		// cout<<seekQueue[0]<<" ";
		output[outputNum] = seekQueue[0];
		outputNum++;
		tmpH = seekQueue[0];
		for (int j = 0; j < seekQueueNum - 1; j++) {
			seekQueue[j] = seekQueue[j + 1];
		}
		seekQueueNum--;
	}
	// cout<<endl;
	// show();
}


void Disk::SSTF() {
	// int seekQueue[20]={98,183,37,122,119,14,124,65,67,198,105,53};
	// int seekQueueNum=12;
	int min = 201;
	int temp = 21;
	int tmpH = curHead;
	//initDisk();
	cout << "SSTF:" << endl;
	while (seekQueueNum > 0) {
		for (int i = 0; i < seekQueueNum; i++) {
			if (abs(tmpH - seekQueue[i]) < min) {
				min = abs(tmpH - seekQueue[i]);
				temp = i;
			}
		}
		// cout<<seekQueue[temp]<<" ";
		output[outputNum] = seekQueue[temp];
		outputNum++;
		tmpH = seekQueue[temp];
		for (int j = temp; j < seekQueueNum - 1; j++) {
			seekQueue[j] = seekQueue[j + 1];
		}
		seekQueueNum--;
		// for(int k=0;k<seekQueueNum;k++){
		//     cout<<seekQueue[k]<<" ";
		// }
		// cout<<endl;
		min = 201;
		temp = 21;
	}
	// cout<<endl;
	// show();
}


void Disk::SCAN() {
	// int seekQueue[20]={98,183,37,122,119,14,124,65,67,198,105,53};
	//seekQueueNum = 12;
	//initDisk();
	int tmpH = curHead;
	cout << "SCAN:" << endl;
	for (int i = 0; i < seekQueueNum; i++) {
		for (int j = 0; j < seekQueueNum - 1; j++) {
			if (seekQueue[j] > seekQueue[j + 1]) {
				int temp = seekQueue[j];
				seekQueue[j] = seekQueue[j + 1];
				seekQueue[j + 1] = temp;
			}
		}
	}
	while (seekQueueNum > 0) {
		int flag = 21;
		for (int i = 0; i < seekQueueNum; i++) {
			if (seekQueue[i] >= tmpH) {
				flag = i;
				break;
			}
		}
		if (flag == 21) {
			// cout<<seekQueue[seekQueueNum-1]<<" ";
			output[outputNum] = seekQueue[seekQueueNum - 1];
			outputNum++;
			tmpH = seekQueue[seekQueueNum - 1];
		}
		else {
			// cout<<seekQueue[flag]<<" ";
			output[outputNum] = seekQueue[flag];
			outputNum++;
			tmpH = seekQueue[flag];
			for (int j = flag; j < seekQueueNum - 1; j++) {
				seekQueue[j] = seekQueue[j + 1];
			}
			if (flag == seekQueueNum - 1) {
				// cout<<disk.trackNum-1<<" ";
				output[outputNum] = trackNum - 1;
				outputNum++;
				tmpH = trackNum - 1;
			}
		}
		seekQueueNum--;
	}
	// cout<<endl;
	// show();
}


void Disk::C_SCAN() {
	// int seekQueue[20]={98,183,37,122,119,14,124,65,67,198,105,53};
	//seekQueueNum = 12;
	//initDisk();
	//cout << "C-SCAN:" << endl;
	int tmpH = curHead;
	for (int i = 0; i < seekQueueNum; i++) {
		for (int j = 0; j < seekQueueNum - 1; j++) {
			if (seekQueue[j] > seekQueue[j + 1]) {
				int temp = seekQueue[j];
				seekQueue[j] = seekQueue[j + 1];
				seekQueue[j + 1] = temp;
			}
		}
	}
	while (seekQueueNum > 0) {
		int flag = 21;
		for (int i = 0; i < seekQueueNum; i++) {
			if (seekQueue[i] >= tmpH) {
				flag = i;
				break;
			}
		}
		// if(flag==21){
		//     cout<<seekQueue[0]<<" ";
		//     disk.tmpH=seekQueue[0];
		//     for(int j=0;j<seekQueueNum-1;j++){
		//         seekQueue[j]=seekQueue[j+1];
		//     }
		// }
		// else{
		// cout<<seekQueue[flag]<<" ";
		output[outputNum] = seekQueue[flag];
		outputNum++;
		tmpH = seekQueue[flag];
		for (int j = flag; j < seekQueueNum - 1; j++) {
			seekQueue[j] = seekQueue[j + 1];
		}
		if (flag == seekQueueNum - 1 && seekQueueNum != 1) {
			// cout<<disk.trackNum-1<<" ";
			output[outputNum] = trackNum - 1;
			outputNum++;
			tmpH = trackNum - 1;
			// cout<<"0 ";
			output[outputNum] = 0;
			outputNum++;
			tmpH = 0;
		}
		// }
		seekQueueNum--;
	}
	// cout<<endl;
	// show();
}


void Disk::LOOK() {
	// int seekQueue[20]={98,183,37,122,119,14,124,65,67,198,105,53};
	// int seekQueueNum=12;
	//initDisk();
	int tmpH = curHead;
	cout << "LOOK:" << endl;
	for (int i = 0; i < seekQueueNum; i++) {
		for (int j = 0; j < seekQueueNum - 1; j++) {
			if (seekQueue[j] > seekQueue[j + 1]) {
				int temp = seekQueue[j];
				seekQueue[j] = seekQueue[j + 1];
				seekQueue[j + 1] = temp;
			}
		}
	}
	while (seekQueueNum > 0) {
		int flag = 21;
		for (int i = 0; i < seekQueueNum; i++) {
			if (seekQueue[i] >= tmpH) {
				flag = i;
				break;
			}
		}
		if (flag == 21) {
			// cout<<seekQueue[seekQueueNum-1]<<" ";
			output[outputNum] = seekQueue[seekQueueNum - 1];
			outputNum++;
			tmpH = seekQueue[seekQueueNum - 1];
		}
		else {
			// cout<<seekQueue[flag]<<" ";
			output[outputNum] = seekQueue[flag];
			outputNum++;
			tmpH = seekQueue[flag];
			for (int j = flag; j < seekQueueNum - 1; j++) {
				seekQueue[j] = seekQueue[j + 1];
			}
		}
		seekQueueNum--;
	}
	// cout<<endl;
	// show();
}


void Disk::C_LOOK() {
	// int seekQueue[20]={98,183,37,122,119,14,124,65,67,198,105,53};
	// int seekQueueNum=12;
	//initDisk();
	cout << "C-LOOK:" << endl;
	int tmpH = curHead;
	for (int i = 0; i < seekQueueNum; i++) {
		for (int j = 0; j < seekQueueNum - 1; j++) {
			if (seekQueue[j] > seekQueue[j + 1]) {
				int temp = seekQueue[j];
				seekQueue[j] = seekQueue[j + 1];
				seekQueue[j + 1] = temp;
			}
		}
	}
	while (seekQueueNum > 0) {
		int flag = 21;
		for (int i = 0; i < seekQueueNum; i++) {
			if (seekQueue[i] >= tmpH) {
				flag = i;
				break;
			}
		}
		if (flag == 21) {
			// cout<<seekQueue[0]<<" ";
			output[outputNum] = seekQueue[0];
			outputNum++;
			tmpH = seekQueue[0];
			for (int j = 0; j < seekQueueNum - 1; j++) {
				seekQueue[j] = seekQueue[j + 1];
			}
		}
		else {
			// cout<<seekQueue[flag]<<" ";
			output[outputNum] = seekQueue[flag];
			outputNum++;
			tmpH = seekQueue[flag];
			for (int j = flag; j < seekQueueNum - 1; j++) {
				seekQueue[j] = seekQueue[j + 1];
			}
		}
		seekQueueNum--;
	}
	// cout<<endl;
	// show();
}


// void show(){
//     for(int i=0;i<disk.outputNum;i++){
//         cout<<disk.output[i]<<" ";
//     }
//     cout<<endl;
// }


