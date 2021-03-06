#include "stdafx.h"
#include "MSerial.h"

#define SERIALREADYREAD_CHECK        0xFA
#define SERIALREADYREAD_FUNCTION     0xDC 
#define SERIALREADYREAD_CHECK_LAST   0xA1

HANDLE MSerial::SerialInit(LPCWSTR COMx, int BaudRate) {

	HANDLE hCom;//串口设备句柄

	DCB dcb = { 0 };
	hCom = CreateFile(COMx,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,//FILE_FLAG_OVERLAPPED,   //同步方式 或 重叠方式   
		0
	);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError();
		printf("Sorry, failed to open the serial\n");
		//return -1;  
		printf("The program will terminate in 3 seconds\n");
		Sleep(3000);
		exit(0);
	}
	else
		printf("The serial is successfully opened in a Baudrate %d!\n", BaudRate);

	dcb.DCBlength = sizeof(DCB);

	if (!GetCommState(hCom, &dcb))
	{
		DWORD dwError = GetLastError();
		return(HANDLE)(-1);
	}

	dcb.BaudRate = BaudRate;   //波特率   
	dcb.ByteSize = 8;          //位数   
	dcb.Parity = NOPARITY;     //奇偶检验   
	dcb.StopBits = ONESTOPBIT;  //停止位数   

	if (!SetCommState(hCom, &dcb))
	{
		DWORD dwError = GetLastError();
		return(HANDLE)(-1);
	}

	if (!PurgeComm(hCom, PURGE_RXCLEAR))   return(HANDLE)(-1);

	SetupComm(hCom, 1024, 1024);
	return hCom;

}

U16 MSerial::SerialRead(HANDLE hCom, void* OutBuf, U16 size) {

	DWORD cnt = 0;
	ReadFile(hCom, OutBuf, size, &cnt, 0);
	return cnt;
}

U16 MSerial::SerialWrite(HANDLE hCom, const void*Buf, U16 size){

	DWORD dw;
	WriteFile(hCom, Buf, size, &dw, NULL);
	return dw;

}

void MSerial::SerialClose(HANDLE hCom) {

	CloseHandle(hCom);

}

void MSerial::SerialClearBuffer(U8*buf, U16 N) {

	memset(buf,0,N);

}

void MSerial::SerialReadyRead(HANDLE hCom) {

	char temp[3] = { SERIALREADYREAD_CHECK ,SERIALREADYREAD_FUNCTION ,SERIALREADYREAD_CHECK_LAST };
	SerialWrite(hCom, temp,3);

}













