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
	private List<HardWareUpEvent> upEventList;
	public HardwareSupport() {
		a8HardwareControlInit();
		upEventList = new ArrayList<HardWareUpEvent>();
	}
	protected void finalize() {
		a8HardwareControlExit();
	}

	public void addEventCallBack(HardWareUpEvent upEvent) {
		upEventList.add(upEvent);
	}

	private void systemCallBack(byte[] Data) {
		if (Data == null)
			return;
		byte[] eventData = new byte[Data.length];
		System.arraycopy(Data, 1, eventData, 0, Data.length - 1);
		byte event = Data[0];
		switch (event) {
		case CallBackState.UI_INFRARED_DEVICE:
			if (eventData[0] == 1) {
				Log.d(TAG, "" + "人物靠近");
				for(HardWareUpEvent temp:upEventList){
					
					temp.someoneCloseEvent();
				}
			}
			break;
		case CallBackState.UI_DOORCARD_DEVICE:

			Log.d(TAG, "不带算法的门卡数据" + eventData.toString());
			for(HardWareUpEvent temp:upEventList){
				
				temp.swipingCardEvent(temp.IC_RAWDATA, eventData);
			}

			break;
		case CallBackState.UI_DOORCARD_DEVICE_ALG:

			if (eventData.length >= 4) {
				for(HardWareUpEvent temp:upEventList){
					temp.swipingCardEvent(temp.IC_ALGDATA, eventData);
				}
			}
			break;

		case CallBackState.UI_OPENDOOR_KEY_DOWN:
			if (eventData[0] == 1) {
				for(HardWareUpEvent temp:upEventList){
					temp.doorLockKeyEvent(temp.KEY_DOWN);
				}
			} else {
				for(HardWareUpEvent temp:upEventList){
					temp.doorLockKeyEvent(temp.KEY_UP);
				}
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
		return a8SetKeyValue(controlHardwareCmd.E_KEY_LIGHT, valve_door_lock, 1);
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

	public int reboot() {
		return a8SetKeyValue(controlHardwareCmd.E_RESTART, null, 0);
	}

	public String getHardWareVersion() {

		byte[] recvData = {0};
		
		recvData = a8GetKeyValue(controlHardwareCmd.E_GET_HARDWARE_VER);
		if (recvData == null)
			return null;
		String recvStr = new String(recvData);
		return recvStr;
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