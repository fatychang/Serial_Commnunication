#include <iostream>
#include "frame.h"
#include "serialcom.h"

#include <chrono>
#include <thread>

#include <stdio.h>

using namespace std;

bool m_stopReceiving = false;
bool m_serialIsOpen = false;
bool flag_isReceiving = false;


LPCSTR port = "\\\\.\\COM4";
DWORD baud = 38400;




int main()
{

	SerialCom * m_serialCom;
	Frame *m_frameRecv;
	Frame *m_frameSend;

	// Define the receiving message length and array (!!!!! be sure to set the correct number of receiving bytes)
	const int revBytesNo = 10; // number of float type data to be received
	float m_sharedArray[revBytesNo] = { 0.0f };

	// Define the sending message length and array
	const int len = 3; // number of float type data to send
	float sharedVal_[len] = { 1.0f, 3.5f, 2.5f };

	// Open serial port 
	m_serialCom = new SerialCom();  // default usb 0 to MCU board
	if (m_serialCom->Open(port, baud)) {						// Open(port, baud)
		m_serialIsOpen = true;
		cout << "Serial com: " << port << " open now" << endl;
	}
	else {
		m_serialIsOpen = false;
		cerr << ">Error: Serial com: " << port << " open failed" << endl;
	}

	// Create frame object for receiving and sending messages
	m_frameRecv = new Frame();
	m_frameSend = new Frame();


	char c;
	int isSend = 0;
	// keep reading, need to set m_stopReceiving to stop loop
	while (!m_stopReceiving && m_serialIsOpen) 
	{
		if (m_serialCom->Read(&c) > 0) { // read one character at one time
			int sz = m_frameRecv->receiveRxFrame(c); // parse received character, sz=0 if decode failed.
			if (sz == revBytesNo * 4) { // if returned size match the defined message length
				if (!flag_isReceiving) {
					flag_isReceiving = true;
				}

				/// Parse message content
				char *rx = m_frameRecv->getRxBuf();
				m_frameRecv->printRxFrame();
				for (int i = 0; i < revBytesNo; ++i) { // ignore cannulation signal in the end byte
					m_sharedArray[i] = (*(float*)(rx + 4 * i));
					//cout << m_sharedArray[i] << ",";
				}

				cout << endl;
			}
		}

		// Send packet every once two loops
		if (isSend == 5)
		{
			// Framing
			int noOfbits = m_frameSend->setTxFrame(sharedVal_, len);			

			// Send the packet
			m_serialCom->Write(m_frameSend->tb, noOfbits);
			
			isSend = 0;

		}
		isSend++;

		// Sleep the listening for 500ms
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));

	}


	// Stop the serial communication
	if (m_serialCom) { delete m_serialCom; m_serialCom = NULL; }
	if (m_frameRecv) { delete m_frameRecv; m_frameRecv = NULL; }

	std::getchar();

	return 0;
}
