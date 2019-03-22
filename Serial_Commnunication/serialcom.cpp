#include "serialcom.h"
#include <iostream>


using namespace std;

SerialCom::SerialCom()
{

}



bool SerialCom::Open(LPCSTR port, DWORD baud)
{

	// Bound the handler
	m_hCom = CreateFile(port,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,		//FILE_FLAG_OVERLAPPED， 异步方式（非阻塞）		0, 同步方式（阻塞）
		NULL
	);

	if (m_hCom == INVALID_HANDLE_VALUE)
	{
		if (DEBUG_CONSOLE) cout << "Failed to open Serial Port!!" << endl;
		return false;
	}
	else
	{
		if (DEBUG_CONSOLE) cout << "Succeed to open Serial Port!!" << endl;

		// Set up dcb parameters
		DCB dcb;
		SetupComm(m_hCom, RX_BUF_SIZE, TX_BUF_SIZE);
		GetCommState(m_hCom, &dcb);
		dcb.BaudRate = baud;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;

		if (!SetCommState(m_hCom, &dcb))
		{
			if (DEBUG_CONSOLE) cout << "Failed to configure Serial Port!!" << endl;
			return false;
		}
		else
		{
			// Successfully make the connection
			PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
		}

		//Set the timeout(ms)
		COMMTIMEOUTS ct;
		ct.ReadIntervalTimeout = MAXDWORD;
		ct.ReadTotalTimeoutConstant = 0;
		ct.ReadTotalTimeoutMultiplier = 0;
		ct.WriteTotalTimeoutConstant = 1;
		ct.WriteTotalTimeoutMultiplier = 1;
		SetCommTimeouts(m_hCom, &ct);

		return true;
	}
}




void SerialCom::Close()
{
	if (INVALID_HANDLE_VALUE != m_hCom)
	{
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
	}
}


DWORD SerialCom::Write(char *data, DWORD length)
{
	DWORD wtn = 0;

	//PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_TXABORT);
	WriteFile(m_hCom, data, length, &wtn, NULL);

	return wtn;
}

DWORD SerialCom::Read(char *c) //, DWORD length)
{
	DWORD rtn = 0;

	ReadFile(m_hCom, c, 1, &rtn, NULL);
	return rtn;
}
