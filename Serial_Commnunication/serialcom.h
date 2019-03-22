#ifndef __SERIALCOM_H
#define __SERIALCOM_H


#include <Windows.h>
#include <process.h>
#include <tchar.h>			// _T()


#define DAC_PORT			_T("\\\\.\\COM4")		// DAC serial port
#define DAC_BAUDRATE		460800
#define IMU_PORT			_T("\\\\.\\COM6")
#define IMU_BAUDRATE		921600

#define TX_BUF_SIZE 128
#define RX_BUF_SIZE 128

#define DEBUG_CONSOLE true

class SerialCom
{
public:

	SerialCom();

	//bool Open(char id);

	/*Open the port and make the connection */
	bool Open(LPCSTR port, DWORD baud);

	/*Close the handle */
	void Close();

	DWORD Write(char *data, DWORD length);
	DWORD Read(char *c);    //, DWORD length);

private:
	HANDLE m_hCom;
	char tb[TX_BUF_SIZE];//transmit buffer
	char rb[RX_BUF_SIZE];//receive buffer
};


#endif
