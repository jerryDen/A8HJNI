package com.welbell.hardware;

import java.util.ArrayList;
import java.util.List;

import com.welbell.hardware.CallBackState;
import com.welbell.hardware.HardWareUpEvent;
import com.welbell.hardware.controlHardwareCmd;

import android.util.Log;

public class HardwareSupport {

	static final String TAG = "HardwareSupport";
	HardWareUpEvent upEvent;
	static private List<HardWareUpEvent> upEventList;

	public HardwareSupport() {
	
		if(upEventList == null )
			upEventList = new ArrayList<HardWareUpEvent>();
	}
	public int init()
	{
		int ret;
		ret = a8HardwareControlInit();
		if(ret < 0)
		{
			Log.e(TAG," fail to a8HardwareControlInit !");
		}
		return ret;
	}
	public int exit()
	{
		return a8HardwareControlExit();
	}
	protected void finalize() {
		Log.d(TAG,"CALL HardwareSupport finalize!");
		//a8HardwareControlExit();
	}

	public void addEventCallBack(HardWareUpEvent upEvent) {
		upEventList.add(upEvent);
		
	}
	public boolean removeEventCallBack(HardWareUpEvent upEvent){
		return upEventList.remove(upEvent);
	}

	private void systemCallBack(byte[] Data) {
		if (Data == null || upEventList == null)
			return;
		byte[] eventData = new byte[Data.length - 1];
		System.arraycopy(Data, 1, eventData, 0, Data.length - 1);
		byte event = Data[0];
		
		switch (event) {
		case CallBackState.UI_INFRARED_DEVICE:
			if (eventData[0] == 1) {

				for (HardWareUpEvent temp : upEventList) {
					if (temp != null)
						temp.someoneCloseEvent();
				}
			}
			break;
		case CallBackState.UI_DOORCARD_DEVICE:{
				byte[] cardData = new byte[Data.length - 2];
				System.arraycopy(eventData, 1, cardData, 0, eventData.length - 1);
				for (HardWareUpEvent temp : upEventList) {
					temp.doorCardBandRawEvent(eventData[0],cardData);
				}
			}
			break;
		case CallBackState.UI_DOORCARD_DEVICE_ALG:{
				int icCardID = 0;
				byte[] cardData = new byte[Data.length - 2];
				System.arraycopy(eventData, 1, cardData, 0, eventData.length - 1);
				if (eventData.length >= 4) {
					for (int i = 0; i < eventData.length; i++) {
						icCardID += eventData[i] & 0xff;
						if (i < eventData.length - 1)
							icCardID <<= 8;
					}
					String strID = Integer.toHexString(icCardID);
					for (HardWareUpEvent temp : upEventList) {
						temp.doorCardBandAlgEvent(eventData[0],strID);
					}
				}
			}
			break;
		case CallBackState.UI_BLUETOOTH_EVENT:{
		
			//byte[] recvData = new byte[eventData.length];
			//Log.d(TAG,"UI_BLUETOOTH_EVENT! len:"+(eventData.length ));
			//System.arraycopy(eventData, 0, recvData, 0, eventData.length);
			String str = new String(eventData);
			if (str != null&&str.length() > 0) {
				for (HardWareUpEvent temp : upEventList) {
					temp.buletoothEvent(str);
				}
			}
		}
		break;
		case CallBackState.UI_OPENDOOR_KEY_DOWN:
			if (eventData[0] == 1) {
				for (HardWareUpEvent temp : upEventList) {
					temp.doorLockKeyEvent(temp.KEY_DOWN);
				}
			} else {
				for (HardWareUpEvent temp : upEventList) {
					temp.doorLockKeyEvent(temp.KEY_UP);
				}
			}
			break;

		case CallBackState.UI_KEYBOARD_EVENT:
			for (HardWareUpEvent temp : upEventList) {

				temp.keyBoardEvent((eventData[0]) & 0xff, eventData[1] & 0xff);
			}
			break;
		
		case CallBackState.UI_MAGNETIC_EVENT:
			for (HardWareUpEvent temp : upEventList) {
				temp.doorMagneticEvent(eventData[0]);
			}
			break;
		
		case CallBackState.UI_PREVENTSEPARATE_EVENT:
			for (HardWareUpEvent temp : upEventList) {
				temp.preventSeparateEvent(eventData[0]);
			}
			break;
		}
	}

	public int doorLockControl(boolean cmd) {
		byte[] valve_door_lock = new byte[1];
		if (cmd == true)
			valve_door_lock[0] = 1;
		else
			valve_door_lock[0] = 0;
		return a8SetKeyValue(controlHardwareCmd.E_DOOR_LOCK, valve_door_lock, 1);
	}

	public int screenBlacklightControl(boolean cmd) {
		byte[] valve_door_lock = new byte[1];
		if (cmd == true)
			valve_door_lock[0] = 1;
		else
			valve_door_lock[0] = 0;
		return a8SetKeyValue(controlHardwareCmd.E_LCD_BACKLIGHT,
				valve_door_lock, 1);
	}

	public int cameraLightControl(boolean cmd) {
		byte[] valve_door_lock = new byte[1];
		if (cmd == true)
			valve_door_lock[0] = 1;
		else
			valve_door_lock[0] = 0;
		return a8SetKeyValue(controlHardwareCmd.E_CAMERA_LIGHT,
				valve_door_lock, 1);
	}
	public int ifcameraLightControl(boolean cmd) {
		byte[] valve_door_lock = new byte[1];
		if (cmd == true)
			valve_door_lock[0] = 1;
		else
			valve_door_lock[0] = 0;
		return a8SetKeyValue(controlHardwareCmd.E_IFCAMERA_LIGHT, valve_door_lock, 1);
	}

	public int keyboardLightControl(boolean cmd) {
		byte[] valve_door_lock = new byte[1];
		if (cmd == true)
			valve_door_lock[0] = 1;
		else
			valve_door_lock[0] = 0;
		return a8SetKeyValue(controlHardwareCmd.E_KEY_LIGHT, valve_door_lock, 1);
	}
	
	public int executeRootShell(String cmdStr) {
		byte[] cmdData = cmdStr.getBytes();
		
		return a8SetKeyValue(controlHardwareCmd.E_EXECURT_SHELL, cmdData,
				cmdData.length);
	}
	public int setBluetoothName(String name) {
		byte[] cmdData = name.getBytes();
		
		return a8SetKeyValue(controlHardwareCmd.E_SET_BLUENAME, cmdData,
				cmdData.length);
	}
	
	public int sendBluetoothStr(String str){
		byte[] cmdData = str.getBytes();
		return a8SetKeyValue(controlHardwareCmd.E_SEND_BLUESTR, cmdData,
				cmdData.length);
	}
	
	public int addAPPtoDaemon(String packageName,String mainActivityName){
		
		byte [] packName = packageName.getBytes();
		byte [] space = {' '};
		byte [] className = mainActivityName.getBytes();
		byte [] total  = new byte[packName.length+className.length+space.length+2];
		System.arraycopy(packName,0,total,0, packName.length);
		System.arraycopy(space,0,total,packName.length, space.length);
		System.arraycopy(className,0,total,packName.length+space.length, className.length);
		
		return a8SetKeyValue(controlHardwareCmd.E_ADD_GUARD,total,
				packName.length+className.length+space.length);
	}
	public int delDaemonServer()
	{
		return a8SetKeyValue(controlHardwareCmd.E_DEL_GUARD,null,0);
	}
	
	public int reboot() {
		return a8SetKeyValue(controlHardwareCmd.E_RESTART, null, 0);
	}
	public int rebootBluetooth(){
		
		return a8SetKeyValue(controlHardwareCmd.E_SET_BLUETOOTH_REBOOT, null, 0); 
	}

	public String getHardWareVersion() {

		byte[] recvData = { 0 };

		recvData = a8GetKeyValue(controlHardwareCmd.E_GET_HARDWARE_VER);
		if (recvData == null)
			return null;
		String recvStr = new String(recvData);
		return recvStr;
	}
	public String getCpuModel() {

		byte[] recvData = { 0 };

		recvData = a8GetKeyValue(controlHardwareCmd.E_GET_CPUMODEL);
		if (recvData == null)
			return null;
		String recvStr = new String(recvData);
		return recvStr;
	}
	
	public String getIdCardUartDev() {

		byte[] recvData = { 0 };

		recvData = a8GetKeyValue(controlHardwareCmd.E_GET_IDCARD_UARTDEV);
		if (recvData == null)
			return null;
		String recvStr = new String(recvData);
		return recvStr;
	}
	
	public int getOptoSensorState()
	{
		byte[] state = {0};
		state = a8GetKeyValue(controlHardwareCmd.E_GET_OPTO_SENSOR_STATE);
		if (state == null)
			return -1;
		return state[0];
	}
	public boolean getBuletoothState(){
		byte[] state = {0};
		state = a8GetKeyValue(controlHardwareCmd.E_GET_BLUETOOTH_STATE);
		if (state == null)
			return false;		
		return  state[0] ==0?true:false;
	}

	native int a8HardwareControlInit();

	native int a8SetKeyValue(int key, byte[] valve, int valueLen);

	native byte[] a8GetKeyValue(int key);

	native int a8HardwareControlExit();

	static {
		try {
			System.loadLibrary("NativeHardwareSupport");
			Log.d(TAG, "loadLibrary成功");
		} catch (Exception e) {
			Log.d(TAG, "loadLibrary失败");
			e.printStackTrace();
		}
	}
}
