package com.welbell.hardware;


public class controlHardwareCmd {

    public static final byte E_DOOR_LOCK = 0x12; // 锁
    /* 控制摄像头灯 例：开灯：buf[] = {0x14,0x01} */
    public static final byte E_CAMERA_LIGHT = 0x14; // 摄像头灯
    /* 控制键盘灯 例：开灯：buf[] = {0x15,0x01} */
    public static final byte E_KEY_LIGHT = 0x15; // 键盘灯
    /* 控制屏幕背光 例：灭屏：buf[] = {0x16,0x01} */
    public static final byte E_LCD_BACKLIGHT = 0x16; // 屏幕背光
    /* 指纹识别 ，方法见fingerprintCmd */
    public static final byte E_RESTART = 0x19; // 重启机器
    public static final byte E_GET_HARDWARE_VER = 0X1A; //获取硬件版本号
    public static final byte E_EXECURT_SHELL = 0x1B; // 以root权限执行shell脚本
}