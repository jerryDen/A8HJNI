#include <linux/stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "WB_virtualHardwareSupport.h"
#include "common/netUdpServer.h"
#include "common/Utils.h"
#include "common/nativeNetServer.h"
#include "common/debugLog.h"
#include "binder/binderClient.h"

typedef struct ThreadArg{
	void *data;
	int len;
}ThreadArg,*pThreadArg;
typedef void *(*ThreadFunc)(pThreadArg);

//应答类命令字
#define NOT_ACK				0XAA
#define ACK_OK				0X61
#define ACK_ERR				0X62
//控制命令字
#define  SIM_CALL_OUT		0X10
#define  SIM_SWIPNG_CARD    0X11
#define  SIM_PEOPHE_CLOSE   0X12

typedef struct T_Data_Head{
	unsigned int  sourceID;//源ID
	unsigned int  desID;	//目标ID
	unsigned int  dtatLen;  //数据长度
	unsigned char dataStart;//数据起始位
}T_Data_Head,*pT_Data_Head;


typedef struct T_Comm_Head {
	unsigned char ackType; //类型:表示主动发送 还是被动接收
	unsigned char cmd;   	//指令：代表这条指令的功能
	unsigned char sequence; //随机码：
	T_Data_Head   dataStart; //数据段
} T_Comm_Head, *pT_Comm_Head;

typedef struct {
	#define UDP_BUF_MAXSIZE  (1024*4)
	unsigned char buf[UDP_BUF_MAXSIZE];
	int len;
	int offset;
} MsgBody, *pMsgBody;


typedef struct {
	VirtualHWops ops;
	pUdpOps udpServer;

#if (USER_BINDER == 1)
	pBinderClientOps binderClient;
#else
	pNativeNetServerOps netClient;
#endif
    WBPirCallBackFunc pirCallBackFunc;
    T_InterruptFunc	 openDoorKeyUpFunc;
    T_InterruptFunc  optoSensorUpFunc;
    DoorCardRecvFunc doorCardRawUpFunc;
    KeyEventUpFunc   keyboardFunc;

}VirtualHWServer, *pVirtualHWServer;

static int setPirUpFunc
			   (struct  VirtualHWops * ops,WBPirCallBackFunc pirCallBack);
static int setOpenDoorKeyUpFunc
				(struct  VirtualHWops * ops ,T_InterruptFunc openKeyCallBack);
static int setOptoSensorUpFunc
				(struct  VirtualHWops * ops ,T_InterruptFunc optoSensorCallBack );
static int setDoorCardRawUpFunc
			(struct  VirtualHWops *ops ,DoorCardRecvFunc ICCardALGCallBack);
static int setKeyBoardUpFunc(struct  VirtualHWops *ops,KeyEventUpFunc keyboardFunc);

static  int udpRecvFunc(unsigned char* data ,unsigned int size);
static int _dialing(char num);
static int _getKeyNum(char ch);
static startThreadwork(ThreadFunc func,pThreadArg arg );
static void * do_callout(pThreadArg arg);
static void callroomNum(char roomNum [4],unsigned char time);
static int swipngCard(char *data,int len );
static int callout( unsigned char room[4],unsigned char  roomnumAndcallTimeS);

static void * do_peopheClose(pThreadArg arg);

static VirtualHWops ops = {
		.setDoorCardRawUpFunc = setDoorCardRawUpFunc,
		.setPirUpFunc = setPirUpFunc,
		.setOpenDoorKeyUpFunc = setOpenDoorKeyUpFunc,
		.setOptoSensorUpFunc = setOptoSensorUpFunc,
		.setKeyBoardUpFunc = setKeyBoardUpFunc,
};
static pVirtualHWServer vHWServer;

static int _getKeyNum(char ch)
{
	struct {
		char keych;
		int keynum;
	}KeyList[12] = {
			{'0',11},{'1',29},{'2',30},{'3',7},{'4',16},{'5',13},
			{'6',8},{'7',15},{'8',12},{'9',9},{'#',10},{'*',14}
	};
	int i;
	for(i = 0 ; i< sizeof(KeyList)/sizeof(KeyList[0]);i++)
	{
		if(KeyList[i].keych == ch  )
			return KeyList[i].keynum;
	}
	return -1;
}

static int _dialing(char num)
{
	char cmdStr[128] = {0};
	char cmdStrSu[128] = {0};
	int keyNum = _getKeyNum(num);
	if(keyNum < 0)
		return -1;
	bzero(cmdStr,sizeof(cmdStr));
	sprintf(cmdStr, "sendevent /dev/input/event3 1 %u 1",keyNum );
#if (USER_BINDER== 1)
	if(vHWServer->binderClient)
	{
		vHWServer->binderClient->runScript(vHWServer->binderClient,cmdStr);
	}
#else
	if(vHWServer->netClient)
	{
		vHWServer->netClient->runScript(vHWServer->netClient,cmdStr);
	}
#endif
	else{
		bzero(cmdStrSu,sizeof(cmdStrSu));
		sprintf(cmdStrSu,"su -c '%s'", cmdStr);
		system(cmdStrSu);
	}
	bzero(cmdStr,sizeof(cmdStr));
	sprintf(cmdStr, "sendevent /dev/input/event3 0 0 0" );
#if (USER_BINDER== 1)
	if(vHWServer->binderClient)
	{
		vHWServer->binderClient->runScript(vHWServer->binderClient,cmdStr);
	}
#else
	if(vHWServer->netClient)
	{
		vHWServer->netClient->runScript(vHWServer->netClient,cmdStr);
	}
#endif
	else{
		bzero(cmdStrSu,sizeof(cmdStrSu));
		sprintf(cmdStrSu,"su -c '%s'", cmdStr);
		system(cmdStrSu);
	}
	bzero(cmdStr,sizeof(cmdStr));
	sprintf(cmdStr, "sendevent /dev/input/event3 1 %u 0",keyNum );
#if (USER_BINDER== 1)
	if(vHWServer->binderClient)
	{
		vHWServer->binderClient->runScript(vHWServer->binderClient,cmdStr);
	}
#else
	if(vHWServer->netClient)
	{
		vHWServer->netClient->runScript(vHWServer->netClient,cmdStr);
	}
#endif

	else{
		bzero(cmdStrSu,sizeof(cmdStrSu));
		sprintf(cmdStrSu,"su -c '%s'", cmdStr);
		system(cmdStrSu);
	}
	bzero(cmdStr,sizeof(cmdStr));
	sprintf(cmdStr, "sendevent /dev/input/event3 0 0 0" );
#if (USER_BINDER== 1)
	if(vHWServer->binderClient)
	{
		vHWServer->binderClient->runScript(vHWServer->binderClient,cmdStr);
	}
#else
	if(vHWServer->netClient)
	{
		vHWServer->netClient->runScript(vHWServer->netClient,cmdStr);
	}
#endif


	else{
		bzero(cmdStrSu,sizeof(cmdStrSu));
		sprintf(cmdStrSu,"su -c '%s'", cmdStr);
		system(cmdStrSu);
	}
	return 0;
}

static startThreadwork(ThreadFunc func,pThreadArg arg )
{
	pthread_t  thread;
	if (pthread_create(&thread,NULL,func,
			(void *)arg) != 0) {
		return -1;
	}

	pthread_detach(thread);
	return 0;
}
static void callroomNum(char roomNum [4],unsigned char time)
{
	const int mapKeyCode[12] = { 11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 14, 28 };
	int i;

	for( i = 0 ;i <4 ; i++)
	{
		LOGD("key = %d",roomNum[i]);
		vHWServer->keyboardFunc(mapKeyCode[roomNum[i]],1);
	    vHWServer->keyboardFunc(mapKeyCode[roomNum[i]],0);
	    usleep(1000*200);
	}
	time = time>110?110:time;
	LOGD("time : %d\n",time);
	sleep(time);
	vHWServer->keyboardFunc(mapKeyCode[11],1);
	vHWServer->keyboardFunc(mapKeyCode[11],0);
}
static void * do_callout(pThreadArg arg)
{
		char room[4] = {0};
		unsigned char time;
		if(arg == NULL ||arg->len != 5)
			return NULL;

		time = (*(char *)arg->data);
		memcpy(room,arg->data+1,4);

		 LOGD("roomnum:");
		 int i;
		 for(i = 0;i < 4;i++)
		 {
			LOGD("%d ",room[i]);
		 }


		callroomNum(room,time);
		if(arg->data != NULL)
			free(arg->data);
		free(arg);
		return NULL;
}

static int callout( unsigned char room[4],unsigned char  roomnumAndcallTimeS)
{
	pThreadArg arg = malloc(sizeof(ThreadArg));
	arg->data = malloc(5);
	memcpy(arg->data,&roomnumAndcallTimeS,1);
	memcpy(arg->data+1,room,4);
	arg->len = 5;
	startThreadwork(do_callout,arg);
	return 0;
}

static int swipngCard(char *data,int len )
{

	if(vHWServer&&data!=NULL&&len>0)
		return vHWServer->doorCardRawUpFunc(IC_CARD,data,len);
	return -1;
}
static void * do_peopheClose(pThreadArg arg)
{

	 vHWServer->pirCallBackFunc(PIR_NEAR);
	 if(arg->data != NULL)
		 free(arg->data);
	 free(arg);
	 return NULL;

}
static int peopheClose(int timeS )
{
	pThreadArg arg = malloc(sizeof(ThreadArg));
	arg->data = malloc(sizeof(int));
	memcpy(arg->data,&timeS,sizeof(int));
	arg->len = sizeof(int);

	if( vHWServer&&vHWServer->pirCallBackFunc)
	{
		return startThreadwork(do_peopheClose,arg);
	}
	return -1;

}
static int setPirUpFunc
			   (struct  VirtualHWops * ops,WBPirCallBackFunc pirCallBack)
{
	pVirtualHWServer pthis = (pVirtualHWServer)ops;
	if(pthis == NULL )
		return -1;
	pthis->pirCallBackFunc = pirCallBack;
	return 0;
}
static int setOpenDoorKeyUpFunc
				(struct  VirtualHWops * ops ,T_InterruptFunc openKeyCallBack)
{
	pVirtualHWServer pthis = (pVirtualHWServer)ops;
	if(pthis == NULL )
		return -1;

	pthis->openDoorKeyUpFunc = openKeyCallBack;
	return 0;

}
static int setOptoSensorUpFunc
				(struct  VirtualHWops * ops ,T_InterruptFunc optoSensorCallBack )
{
	pVirtualHWServer pthis = (pVirtualHWServer)ops;
	if(pthis == NULL )
		return -1;
	pthis->optoSensorUpFunc = optoSensorCallBack;
	return 0;
}
static int setKeyBoardUpFunc(struct  VirtualHWops *ops,KeyEventUpFunc keyboardFunc)
{
	pVirtualHWServer pthis = (pVirtualHWServer)ops;
		if(pthis == NULL )
			return -1;
	pthis->keyboardFunc = keyboardFunc;
	return 0;
}
static int setDoorCardRawUpFunc
			(struct  VirtualHWops *ops ,DoorCardRecvFunc ICCardALGCallBack)
{
	pVirtualHWServer pthis = (pVirtualHWServer)ops;
	if(pthis == NULL )
		return -1;
	pthis->doorCardRawUpFunc = ICCardALGCallBack;
	return 0;
}

static  int udpRecvFunc(unsigned char* data ,unsigned int size)
{
	if(data == NULL || size< sizeof(T_Comm_Head))
		return size;
	T_Comm_Head * pHead = (T_Comm_Head *)data;
	getUtilsOps()->printData(data,size);
	pHead->dataStart.dtatLen = ntohl(pHead->dataStart.dtatLen);
	int i;
	if(pHead->ackType == NOT_ACK)
	{
		switch(pHead->cmd)
		{
			case SIM_CALL_OUT:{
				 LOGD("SIM_CALL_OUT!");
				 getUtilsOps()->printData(&pHead->dataStart.dataStart,pHead->dataStart.dtatLen);
				 unsigned char room[4] = {0};
				 unsigned char time = 0;
				 memcpy(&time,(char *)(&pHead->dataStart.dataStart),1);
				 memcpy(room,(char *)(&pHead->dataStart.dataStart+1),4);


				 callout(room,time);
				}
				break;
			case SIM_SWIPNG_CARD:
				 LOGD("SIM_SWIPNG_CARD len:%d",pHead->dataStart.dtatLen);
					 swipngCard(&pHead->dataStart.dataStart,pHead->dataStart.dtatLen);
				break;
			case SIM_PEOPHE_CLOSE:
					 LOGD("SIM_PEOPHE_CLOSE time:%d",pHead->dataStart.dataStart);
				     peopheClose(pHead->dataStart.dataStart);
				break;
			default:
				break;
		}
	}
	return size;
}


static MsgBody UdpBuildMsg(unsigned char ackType, unsigned char cmd, const unsigned char *pData, int dataLen) {
	MsgBody tmpMsg;
	//包结构长度+数据长度+校验位
	int totalLen = sizeof(T_Comm_Head) + dataLen;
	T_Comm_Head * pHead = (T_Comm_Head *) tmpMsg.buf;
	static unsigned char seq = 0;
	pHead->ackType = ackType;
	pHead->cmd = cmd;
	pHead->sequence = seq++;
	if (pData != NULL && dataLen > 0) {
		memcpy(&(pHead->dataStart.dataStart), pData, dataLen);
		pHead->dataStart.dtatLen = htonl(dataLen);
	}
	//最后一位是校验位
	tmpMsg.buf[totalLen - 1] = getUtilsOps()->NByteCrc8(0, (unsigned char *) pHead, totalLen - 1);
	tmpMsg.len = totalLen;
	return tmpMsg;
}

pVirtualHWops crateVirtualHWServer(void)
{
	if(vHWServer)
		return (pVirtualHWops)vHWServer;
	vHWServer = malloc(sizeof(VirtualHWServer));
	if(vHWServer == NULL )
		goto fail0;
	vHWServer->udpServer =  createUdpServer(19999);
	if(vHWServer->udpServer == NULL )
		goto fail1;
	vHWServer->udpServer->setHandle(vHWServer->udpServer,udpRecvFunc,NULL,NULL);
#if (USER_BINDER== 1)
	vHWServer->binderClient = binder_getServer();
#else
	vHWServer->netClient = createNativeNetServer();
#endif
	vHWServer->ops = ops;
	return (pVirtualHWops)vHWServer;
fail1:
	free(vHWServer);
fail0:
	return NULL;

}
void destroyVirtualHWServer(pVirtualHWops *server)
{
#if (USER_BINDER== 1)
	if(vHWServer->binderClient)
		binder_releaseServer(&vHWServer->binderClient);
#else

#endif

	if(vHWServer->udpServer)
		destroyUdpServer(&vHWServer->udpServer);
	free(vHWServer);
	*server = NULL;
}
