#include "frame.h"
#include <iostream>


unsigned int Frame::CRC16CCITT_Calculate(char* s, unsigned char len, unsigned int crc)
{
	//CRC Order: 16
	//CCITT(recommendation) : F(x)= x16 + x12 + x5 + 1
	//CRC Poly: 0x1021
	//Operational initial value:  0xFFFF
	//Final xor value: 0
	unsigned char i, j;
	for (i = 0; i < len; i++, s++) {
		crc ^= ((unsigned int)(*s) & 0xFF) << 8;
		for (j = 0; j < 8; j++) {
			if (crc & 0x8000) {
				crc = (crc << 1) ^ 0x1021;
			}
			else {
				crc <<= 1;
			}
		}
	}
	return (crc & 0xFFFF);  //truncate last 16 bit
}


Frame::Frame() :TxN(0), RxN(0), rState(SKIP) {}

void Frame::printRxFrame()
{
	printf("Rx data buffer: ");
	for (int j = 0; j < RxN; j++) {
		printf("%02X ", (unsigned char)rb[j]);
	}
	printf("\n");
}

void Frame::printTxFrame()
{
	printf("Tx frame buffer: ");
	for (int j = 0; j < TxN; j++) {
		printf("%02X ", (unsigned char)tb[j]);
	}
	printf("\n");
}


int Frame::setTxFrame(char* d, int n)
{
	unsigned int txcrc = 0xFFFF;    //initialize crc
	char c;
	int i = 0, j = 0;

	tb[i++] = STX;    // start of the frame
	for (j = 0; j < n; j++) {
		c = d[j];
		if ((c == STX) || (c == ETX) || (c == DLE)) {
			tb[i++] = (DLE);    // add DLE
		}
		tb[i++] = c;
	}

	tb[i++] = (ETX);    //end of frame

	txcrc = CRC16CCITT_Calculate(d, n, txcrc);  //calculate crc
	tb[i++] = txcrc & 0xFF;
	tb[i++] = (txcrc >> 8) & 0xFF;

	TxN = i;
	return TxN;
}

int Frame::setTxFrame(float sharedVal_[], int len)
{

	// temporal variables
	char c;
	char b[4];
	int i = 0, j = 0;
	unsigned int crc = 0xFFFF;

	tb[i++] = STX;			// frame starts with STX


	int cnt;
	for (cnt = 0; cnt < len; ++cnt) {		// only send the first two (beta, gama);
		*(float*)(b) = sharedVal_[cnt];       // b: 4 bytes
		for (j = 0; j < 4; j++) {
			c = (char)b[j];	// c: 1 byte

			if ((c == STX) || (c == ETX) || (c == DLE)) {
				tb[i++] = (DLE);	// Byte stuffing
			}
			tb[i++] = (c);
		}
		crc = CRC16CCITT_Calculate(b, 4, crc);

	}

	tb[i++] = (ETX);			//end of frame

	tb[i++] = crc & 0xFF;		// append crc code
	tb[i++] = (crc >> 8) & 0xFF;

	// Print TxFrame
	//this->printTxFrame();

	//std::cout << "txN:" << TxN << std::endl;
	return TxN = i;
}

int Frame::receiveRxFrame(char c)
{
	static char b;
	unsigned int crc;
	unsigned int rxcrc = 0xFFFF;    //initialize CRC

	switch (rState) {   // parsing
	case SKIP:
		//std::cout << "skip" << std::endl;
		if (c == STX) { rState = RECEIVING; RxN = 0; }//printf("start.\n");break;}
		break;
	case RECEIVING:
		//std::cout << "receiving" << std::endl;
		if (c == STX) { rState = RECEIVING; RxN = 0; }//printf("receive stx.\n");
		else if (c == ETX) { rState = RXCRC1; }//printf("receive etx.\n");
		else if (c == DLE) { rState = ESCAPE; }//printf("receive dle.\n");}
		else { rb[RxN++] = c; }//printf("receiving.\n");
		break;
	case ESCAPE:
		//std::cout << "escape" << std::endl;
		rb[RxN++] = c;
		rState = RECEIVING;//printf("escape.\n");
		break;
	case RXCRC1:
		//std::cout << "rxcrc1" << std::endl;
		b = c;
		rState = RXCRC2; //printf("crc1.\n");
		break;
	case RXCRC2:
		//std::cout << "rxcrc2" << std::endl;
		rState = SKIP;//printf("crc2\n");
		crc = ((int)c << 8 | ((int)b & 0xFF)) & 0xFFFF;   //get received crc
		rxcrc = CRC16CCITT_Calculate(rb, RxN, rxcrc);   //calculate crc
		if (rxcrc == crc) { // if crc is correct, a full frame is received
			//printf("crc right\n");
			this->printRxFrame();//setRxData();
			return RxN; // return the number of receivng bytes
		}
		else {    // incorrect crc, discard the frame
			RxN = 0;//printf("crc wrong.\n");
		}
		break;
	}
	return 0;
}
