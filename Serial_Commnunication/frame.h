// Byte stuffing- sending and receiving frames
// Author: Yan Naing Aye    - 2016 Oct
// Modified by Feng Chen, write in a class    - 2016 Nov

#ifndef FRAME_H
#define FRAME_H

#include <stdio.h>
#include <vector>

#define STX 0x02    // The head of the frame
#define ETX 0x03    // The end of the frame
#define DLE 0x10    // Special flag for the data which is conflict with the STX, ETX, DLE

#define TX_BUF_SIZE 128 // Size of transmit buffer (bytes)
#define RX_BUF_SIZE 128 // Size of receive buffer

enum RX_STATE { SKIP, RECEIVING, ESCAPE, RXCRC1, RXCRC2 };    // Receive state, used for parse the receiving frame

/**
 * @brief The Frame class for transmitting and receiving frames based on Serial Communication.
 */
class Frame {
	RX_STATE rState;    // current receiving parse state

public:
	int TxN;                    //number of transmitting bytes
	int RxN;                    //number of receiving bytes

	char tb[TX_BUF_SIZE];       //transmit buffer
	char rb[RX_BUF_SIZE];       //receiving data

public:
	Frame();

	/*!
	 * \brief Frame::CRC16CCITT_Calculate
	 * \param s pointer to input char string
	 * \param len string len (maximum 255)
	 * \param crc initial CRC value
	 * \return calculated CRC
	 */
	unsigned int CRC16CCITT_Calculate(char* s, unsigned char len, unsigned int crc);

	/*!
	 * \brief Set the transfer frame (a full package of message)
	 * \param d the array only contains valid contents need to be sent
	 * \param n the size of the array d
	 * \return the size of the frame after packaging (including start char, end char etc.)
	 */
	int setTxFrame(char* d, int n);
	int setTxFrame(float sharedVal_[], int len);


	/*!
	 * \brief Get the size of a full transfered buffer, including start char, etc.
	 * \return the size of a full transfered buffer.
	 */
	int getTxN() { return TxN; }   // get number of transmitting bytes

	/*!
	 * \brief Get the size of a received buffer, excluding start char, etc.
	 * \return the size of a received buffer.
	 */
	int getRxN() { return RxN; }   // get number of transmitting bytes

	/*!
	 * \brief Parse the received character.
	 * \param c the copy of the received character which is parsed based on the defined package format
	 * \return the number of bytes of the received full packages.
	 */
	int receiveRxFrame(char c); // get receiving frame from received char

	// Get buffer
	/*!
	 * \brief Get transfer buffer.
	 * \return char pointer of the transfer buffer
	 */
	char* getTxBuf() { return tb; }

	/*!
	 * \brief Get receive buffer.
	 * \return char pointer of the receive buffer
	 */
	char* getRxBuf() { return rb; }

	// print frame
	/*!
	 * \brief Print receive buffer.
	 */
	void printRxFrame();

	/*!
	 * \brief Print transfer buffer.
	 */
	void printTxFrame();

};

#endif
