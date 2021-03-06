#include "hwInterface/hwInterfaceManage.h"
#include "hwInterface/hwInterfaceConfig.h"
#include "common/Utils.h"
#include "common/debugLog.h"
#include <stddef.h>
static CPU_VER cpuVer;

#define RK_P0(num) (0+num)
#define RK_P1(num) (32+num)
#define RK_P2(num) (64+num)
static int PA(int num) {
	if (cpuVer == A20 ||cpuVer == A64)
		return num;
	else if(cpuVer == RK3368||cpuVer == RK3128||cpuVer == RK3288)
		return num;
	return -1;
}
static int PB(int num) {
	if (cpuVer == A20)
		return num + 24;
	else if (cpuVer == A64)
		return num + 32;
	else if(cpuVer == RK3368||cpuVer == RK3128||cpuVer == RK3288)
		return num + 8;
	return -1;
}
static int PC(int num) {
	if (cpuVer == A20)
		return num + 54;
	else if (cpuVer == A64)
		return num + 64;
	else if(cpuVer == RK3368||cpuVer == RK3128||cpuVer == RK3288)
		return num + 16;
	return -1;
}
static int PD(int num) {
	if (cpuVer == A20)
		return num + 85;
	else if (cpuVer == A64)
		return num + 96;
	else if(cpuVer == RK3368||cpuVer == RK3128||cpuVer == RK3288)
			return num + 24;
	return -1;
}
static int PE(int num) {
	if (cpuVer == A20)
		return num + 119;
	else if (cpuVer == A64)
		return num + 128;
	return -1;
}
static int PF(int num) {
	if (cpuVer == A20)
		return num + 137;
	else if (cpuVer == A64)
		return num + 160;
	return -1;
}
static int PG(int num) {
	if (cpuVer == A20)
		return num + 149;
	else if (cpuVer == A64)
		return num + 192;
	return -1;
}
int PH(int num) {
	if (cpuVer == A20)
		return num + 167;
	else if (cpuVer == A64)
		return num + 224;
	return -1;
}

static int PI(int num) {
	if (cpuVer == A20)
		return num + 201;
	else if (cpuVer == A64)
		return num + 256;
	return -1;
}

static int PJ(int num) {
	if (cpuVer == A20)
		return -1;
	else if (cpuVer == A64)
		return num + 288;
	return -1;
}
static int Pk(int num) {
	if (cpuVer == A20)
		return -1;
	else if (cpuVer == A64)
		return num + 320;
	return -1;
}
static int PL(int num) {
	if (cpuVer == A20)
		return -1;
	else if (cpuVer == A64)
		return num + 352;
	return -1;
}
static int PM(int num) {
	if (cpuVer == A20)
		return -1;
	else if (cpuVer == A64)
		return (num + 384);
	return -1;
}
static int PN(int num) {
	if (cpuVer == A20)
		return -1;
	else if (cpuVer == A64)
		return (num + 416);
	return -1;

}
static int PO(int num) {
	if (cpuVer == A20)
		return -1;
	else if (cpuVer == A64)
		return (num + 448);
	return -1;
}

static int getDoorLockPin(void) {
	if (cpuVer == A20)
		return PG(3);
	else if (cpuVer == A64)
		return PB(2); //未设定
	else if(cpuVer == RK3368)
		return PB(1);
	else if(cpuVer == RK3128)
		return RK_P0(PD(0));
	return -1;
}
static int getOpenDoorKeyPin(void) {
	if (cpuVer == A20)
		return PG(2);
	else if (cpuVer == A64)
		return PB(3);
	else if(cpuVer == RK3128)
			return RK_P1(PA(3));
	return -1;
}

static int getLightSensorPin(void) {
	if (cpuVer == A20)
		return PI(15);
	else if (cpuVer == A64)
		return PB(4); //未设定
	else if(cpuVer==RK3128)
		return RK_P0(PB(5));
	return -1;
}
static int getCameraLightPin(void) {
	if (cpuVer == A20)
		return PH(11);
	else if (cpuVer == A64)
		return PL(11);
	else if(cpuVer==RK3128)
		return RK_P0(PB(6));
	return -1;
}
static int getIFCameraLightPin(void){
	if (cpuVer == A20)
		return  PH(0);
	else if (cpuVer == A64)
		return PB(7);
	else if(cpuVer==RK3128)
		return RK_P0(PB(6));
	return -1;

}
static int getKeyLightPin(void) {
	if (cpuVer == A20)
		return PH(0);
	else if (cpuVer == A64)
		return PL(12);
	else if(cpuVer==RK3128)
		return RK_P2(PD(5));
	return -1;
}
static int getLcdSwichPin(void) {
	if (cpuVer == A20)
		return PH(7);
	else if (cpuVer == A64)
		return PD(23);
	else if(cpuVer == RK3128)
			return RK_P1(PA(4));
	return -1;
}
static int getRestartPin(void) {
	if (cpuVer == A20)
		return PG(5);
	else if (cpuVer == A64)
		return PG(5);
	return -1;
}
static int getPirPin(void) {
	if (cpuVer == A20)
		return PH(12);
	else if (cpuVer == A64)
		return PL(7);
	else if(cpuVer==RK3128)
		return RK_P0(PB(3));
	return -1;
}
//门磁
static int getDoorMagneticPin(void){
	if (cpuVer == A20)
		return PG(1);
	else if (cpuVer == A64)
		return PE(17);
	else if(cpuVer==RK3128)
		return RK_P1(PA(2));
	return -1;
}
static int getSecurityPin(void){
	if (cpuVer == A20)
		return -1;//PH(1);
	else if (cpuVer == A64)
		return PB(6);
	else if(cpuVer==RK3128)
		return RK_P0(PC(1));
	return -1;
}
static char *getBluetoothUART(void)
{
	if (cpuVer == A20)
			return NULL;
	else if (cpuVer == A64)
			return "/dev/ttyS1";
	return NULL;
}
static char *getDoorCardUART(void) {
	if (cpuVer == A20)
		return "/dev/ttyS6";
	else if (cpuVer == A64)
		return "/dev/ttyS3";
	else if (cpuVer == RK3128)
		return "/dev/ttyS1";
	return NULL;
}
static char * getIdCardUART(void)
{
	if (cpuVer == A20)
		#ifdef USER_ID
			return "/dev/ttyS6";
    	#else
			return NULL;
		#endif
	else if(cpuVer == A64)
			return "/dev/ttyS2";
	else if(cpuVer == RK3368 )
			return "/dev/ttyS0";
	else if(cpuVer == RK3128 )
			return "/dev/ttyS0";
	return NULL;
}
static DOOR_CARD_MODULE getDoorType(void) {

	#ifdef USER_FM1702NL
		return FM1702NL;
	#endif
	#ifdef USER_ZLG600A
		return  ZLG600A;
	#endif

	return -1;
}
static char *getRs485UART(void){
	if (cpuVer == A20){
		return "/dev/ttyS5";
	}
	else if(cpuVer == A64){
		return "/dev/ttyS0";
	}
	return NULL;
}
static int getRs485controlPin(void){
	if (cpuVer == A20){
		return PH(6);
	}
	else if(cpuVer == A64){
		return PH(8);
	}
	return -1;
}
static int getRedLedPin(void){
	if(cpuVer == A64){
		return PL(4);
	}
	return -1;

}
static int getGreenLedPin(void){
	if(cpuVer == A64){
		return PL(5);
	}
	return -1;

}
static int getBlueLedPin(void){
	if(cpuVer == A64){
		return PL(6);
	}
	return -1;
}
static char * getTemperatureDetectionUART(void){
	return NULL;
}


static HwInterfaceOps ops = {
		.getDoorLockPin = getDoorLockPin,//控制门锁
		.getRedLedPin = getRedLedPin,//获取红色LED PIN
		.getGreenLedPin = getGreenLedPin,//获取绿色LED PIN
		.getBlueLedPin = getBlueLedPin,//获取蓝色LED PIN
		.getOpenDoorKeyPin = getOpenDoorKeyPin,//控制内部开门按钮
		.getLightSensorPin = getLightSensorPin,//摄像头光敏反馈
		.getCameraLightPin = getCameraLightPin,//控制摄像头灯
		.getIFCameraLightPin = getIFCameraLightPin,//控制摄像头灯
		.getKeyLightPin = getKeyLightPin,//控制键盘灯
		.getLcdSwichPin = getLcdSwichPin,//控制屏幕背光
		.getRestartPin = getRestartPin,//重启机器
		.getPirPin = getPirPin,//人体红外按钮
		.getDoorCardUART = getDoorCardUART,//获取门卡
		.getDoorType = getDoorType,//获取门卡模块类型
		.getDoorMagneticPin = getDoorMagneticPin,//获取门磁反馈
		.getSecurityPin = getSecurityPin,//获取防拆按钮反馈
		.getIdCardUART = getIdCardUART,//获取身份证对应的串口号
		.getBluetoothUART = getBluetoothUART,
		.getRs485UART = getRs485UART,
		.getRs485controlPin = getRs485controlPin,
		.getTemperatureDetectionUART = getTemperatureDetectionUART,
};

pHwInterfaceOps crateHwInterfaceServer(void) {
	cpuVer = getUtilsOps()->getCpuVer();
	pHwInterfaceOps pops = NULL;
	LOGD("cpu ver:%d",cpuVer);
	pops = getHwInterfaceConfigServer();
	if(pops != NULL)
		return pops;
	return &ops;
}
