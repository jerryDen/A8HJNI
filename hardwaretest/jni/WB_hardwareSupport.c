#include <fcntl.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "WB_hardwareSupport.h"
#include "WB_icDoorCard.h"
#include "WB_pirSupport.h"
#include "common/nativeNetServer.h"
#include "gpio/gpioServer.h"
#include "gpio/hwInterfaceManage.h"
#include "common/debugLog.h"

typedef  struct WB_hardWareServer {
	WB_hardWareOps  ops;
	pGpioOps doorServer;
	pGpioOps openDoorKeyServer;
	pGpioOps optoSensorServer;
	pGpioOps IFCamerLightServer;
	pGpioOps LCDLightServer;
	pGpioOps camerLightServer;
	pGpioOps keyboardLightServer;
	//pBinderClientOps binderClient;
	pNativeNetServerOps netServer;
	pIcDoorCardOps  doorCardServer;
	pWBPir_ops	pirServer;
	pHwInterfaceOps interfaceOps;
	pWB_KeyBoardOps keyBoardServer;

}WB_hardWareServer,*pWB_hardWareServer;


static int controlDoor(struct  WB_hardWareOps *,ControlCmd);
static int controlLCDLight(struct  WB_hardWareOps *,ControlCmd);
static int controlIFCameraLight(struct  WB_hardWareOps *,ControlCmd);
static int controlCameraLight(struct  WB_hardWareOps *,ControlCmd);
static int controlKeyboardLight(struct  WB_hardWareOps *,ControlCmd);
static int sendShellCmd(struct  WB_hardWareOps *,const char *);
static int reboot (struct  WB_hardWareOps *);
static int setPirUpFunc(struct  WB_hardWareOps * ops,WBPirCallBackFunc pirUpFunc);
static int setOpenDoorKeyUpFunc(struct  WB_hardWareOps *,T_InterruptFunc);
static int setOptoSensorUpFunc(struct  WB_hardWareOps *,T_InterruptFunc);
static int setIcCardRawUpFunc(struct  WB_hardWareOps * ops ,IcRecvFunc rawUpFunc);
static int getOptoSensorState(struct  WB_hardWareOps *ops );
static int setKeyboardEventUpFunc(struct  WB_hardWareOps * ops,KeyEventUpFunc func);
static WB_hardWareOps ops = {
		.controlDoor = controlDoor,
		.controlIFCameraLight = controlIFCameraLight,
		.controlCameraLight = controlCameraLight,
		.controlKeyboardLight = controlKeyboardLight,
		.controlLCDLight = controlLCDLight,
		.sendShellCmd = sendShellCmd,
		.reboot = reboot,
		.setPirUpFunc = setPirUpFunc,
		.setOpenDoorKeyUpFunc = setOpenDoorKeyUpFunc,
		.getOptoSensorState = getOptoSensorState,
		.setIcCardRawUpFunc = setIcCardRawUpFunc,
		.setKeyboardEventUpFunc = setKeyboardEventUpFunc,
};
static int setKeyboardEventUpFunc(struct  WB_hardWareOps * ops,KeyEventUpFunc func)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
	if(hardWareServer == NULL || hardWareServer->keyBoardServer == NULL)
		goto fail0;
	return hardWareServer->keyBoardServer->setKeyEventUpFunc
		(hardWareServer->keyBoardServer,func);
fail0:
	return -1;
}
static int setIcCardRawUpFunc(struct  WB_hardWareOps * ops ,IcRecvFunc rawUpFunc)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
	if(hardWareServer == NULL)
		return -1;

	hardWareServer->doorCardServer = crateIcDoorCardOpsServer(
			hardWareServer->interfaceOps->getIcCardUART(),rawUpFunc);
	if(hardWareServer->doorCardServer == NULL )
		return -1;
	return 0;
}

static int controlDoor(struct  WB_hardWareOps * ops,ControlCmd cmd)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
	if(hardWareServer == NULL)
		return -1;
	return hardWareServer->doorServer->setOutputValue(
				hardWareServer->doorServer,cmd);
}
static int controlLCDLight(struct  WB_hardWareOps *ops ,ControlCmd cmd)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
	if(hardWareServer == NULL)
				return -1;
	return hardWareServer->LCDLightServer->setOutputValue(
					 hardWareServer->LCDLightServer,cmd);
}
static int controlIFCameraLight(struct  WB_hardWareOps * ops,ControlCmd cmd)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
		if(hardWareServer == NULL)
							return -1;
		return hardWareServer->IFCamerLightServer->setOutputValue(
				 hardWareServer->IFCamerLightServer,cmd);

}
static int controlCameraLight(struct  WB_hardWareOps *ops,ControlCmd cmd)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
	if(hardWareServer == NULL)
						return -1;
	return hardWareServer->camerLightServer->setOutputValue(
			 hardWareServer->camerLightServer,cmd);

}
static int controlKeyboardLight(struct  WB_hardWareOps *ops,ControlCmd cmd)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
		if(hardWareServer == NULL)
					return -1;
		return hardWareServer->keyboardLightServer->setOutputValue(
				hardWareServer->keyboardLightServer,cmd);
}
static int sendShellCmd(struct  WB_hardWareOps *ops,const char * cmd)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
	if(hardWareServer == NULL)
				return -1;
	if(hardWareServer->netServer){
		return hardWareServer->netServer->runScript(
					hardWareServer->netServer,cmd);
	}else
	{
		char cmdStr[128] = {0};
		sprintf(cmdStr,"su -c %s",cmd);
		LOGD("cmd:%s",cmdStr);
		return  system(cmdStr);
	}
}


static int reboot (struct  WB_hardWareOps * ops)
{

	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
	if(hardWareServer == NULL)
			return -1;
	if(hardWareServer->netServer){
		return hardWareServer->netServer->runScript(
				hardWareServer->netServer,"reboot");
	}else {
		return system("su -c reboot");
	}
}
static int setPirUpFunc(struct  WB_hardWareOps * ops,WBPirCallBackFunc pirUpFunc)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
	if(hardWareServer == NULL)
		return -1;
	hardWareServer->pirServer =  crateWBPirServer(
			hardWareServer->interfaceOps->getPirPin(),pirUpFunc);
	if(hardWareServer->pirServer == NULL)
		return -1;
	return 0;
}
static int getOptoSensorState(struct  WB_hardWareOps *ops )
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
	if(hardWareServer == NULL )
		goto fail0;
	if(hardWareServer->optoSensorServer == NULL)
		goto fail0;
	return hardWareServer->optoSensorServer->getInputValue
			(hardWareServer->optoSensorServer);
fail0:
	return -1;
}
static int setOpenDoorKeyUpFunc(struct  WB_hardWareOps * ops,T_InterruptFunc OpenDoorKeyUpFunc)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;
	hardWareServer->openDoorKeyServer = gpio_getServer(
			hardWareServer->interfaceOps->getOpenDoorKeyPin());
	if(hardWareServer->openDoorKeyServer == NULL)
		return -1;
	hardWareServer->openDoorKeyServer->setInterruptFunc(
			hardWareServer->openDoorKeyServer,OpenDoorKeyUpFunc,NULL,BOTH );

	return 0;

}
static int setOptoSensorUpFunc(struct  WB_hardWareOps *ops,T_InterruptFunc optoSensorUpFunc)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)ops;

	if(hardWareServer->optoSensorServer == NULL)
		return -1;
	hardWareServer->optoSensorServer->setInterruptFunc(
			hardWareServer->optoSensorServer,optoSensorUpFunc,NULL,BOTH );

	return 0;
}
pWB_hardWareOps crateHardWareServer(CPU_VER ver)
{
	pWB_hardWareServer hardWareServer = malloc(sizeof(WB_hardWareServer ));
	if(hardWareServer == NULL ){
		LOGE("fail to malloc hardWareServer!");
		goto fail0;
	}
	bzero(hardWareServer,sizeof(WB_hardWareServer));
	hardWareServer->interfaceOps = crateHwInterfaceServer(ver);
	hardWareServer->doorServer = gpio_getServer(
			hardWareServer->interfaceOps->getDoorLockPin());
	if(hardWareServer->doorServer == NULL ){
		LOGE("fail to malloc doorServer!");
		goto fail1;
	}
	hardWareServer->openDoorKeyServer = gpio_getServer(
			hardWareServer->interfaceOps->getOpenDoorKeyPin());;
	if(hardWareServer->openDoorKeyServer == NULL){
		LOGE("fail to malloc openDoorKeyServer!");
		goto fail2;
	}
	hardWareServer->IFCamerLightServer = gpio_getServer(
			hardWareServer->interfaceOps->getKeyLightPin());

	if(hardWareServer->IFCamerLightServer == NULL ){
		LOGE("fail to malloc IFCamerLightServer!");
		goto fail3;
	}
	hardWareServer->keyboardLightServer = hardWareServer->IFCamerLightServer;

	hardWareServer->camerLightServer = gpio_getServer(
			hardWareServer->interfaceOps->getCameraLightPin());
	if(hardWareServer->camerLightServer == NULL){
		LOGE("fail to malloc camerLightServer!");
		goto fail4;
	}
	hardWareServer->LCDLightServer = gpio_getServer(
			hardWareServer->interfaceOps->getLcdSwichPin());
	if(hardWareServer->LCDLightServer == NULL){
		LOGE("fail to malloc LCDLightServer!");
		goto fail5;
	}
	hardWareServer->optoSensorServer = gpio_getServer(
			hardWareServer->interfaceOps->getLightSensorPin());
	if(hardWareServer->optoSensorServer == NULL)
	{
		goto fail6;
	}
	hardWareServer->keyBoardServer = createKeyBoardServer("dev/input/event");
	if(hardWareServer->keyBoardServer == NULL)
	{
		goto fail7;
	}


	hardWareServer->netServer = createNativeNetServer();
	if(hardWareServer->netServer == NULL){
		LOGE("fail to malloc netServer!");
	//	goto fail7;
	}

	hardWareServer->ops = ops;
	return  (pWB_hardWareOps)hardWareServer;

fail7:
	gpio_releaseServer(&hardWareServer->optoSensorServer);
fail6:
	gpio_releaseServer(&hardWareServer->LCDLightServer);
fail5:
	gpio_releaseServer(&hardWareServer->camerLightServer);
fail4:
	gpio_releaseServer(&hardWareServer->IFCamerLightServer);
fail3:
	gpio_releaseServer(&hardWareServer->openDoorKeyServer);
fail2:
	gpio_releaseServer(&hardWareServer->doorServer);
fail1:
	free(hardWareServer);
fail0:
	return NULL;
}

void destroyHardWareServer(pWB_hardWareOps *ops)
{
	pWB_hardWareServer hardWareServer  = (pWB_hardWareServer)*ops;
	if(hardWareServer == NULL)
		return ;
	if(hardWareServer->keyboardLightServer)
		gpio_releaseServer(&hardWareServer->keyboardLightServer);
	if(hardWareServer->camerLightServer)
		gpio_releaseServer(&hardWareServer->camerLightServer);
	if(hardWareServer->IFCamerLightServer)
		gpio_releaseServer(&hardWareServer->IFCamerLightServer);
	if(hardWareServer->openDoorKeyServer)
		gpio_releaseServer(&hardWareServer->openDoorKeyServer);
	if(hardWareServer->doorServer)
		gpio_releaseServer(&hardWareServer->doorServer);
	if(hardWareServer->pirServer)
		destroyWBPirServer(&hardWareServer->pirServer);
	if(hardWareServer->doorCardServer)
		destroyIcDoorCardOpsServer(&hardWareServer->doorCardServer);
	if(hardWareServer->keyBoardServer)
		destroyKeyBoardServer(&hardWareServer->keyBoardServer);
	if(hardWareServer->netServer)
		destroyNativeNetServer(&hardWareServer->netServer);

	free(hardWareServer);
		*ops = NULL;

}





































