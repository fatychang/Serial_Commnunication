# Serial_Commnunication
Serial comm with data framing

This solution provides a serial communication with both receiving and sending.
The port and the baud rate is required to modity so as to match the other end of the communication which you can find in the main.cpp file.

Frame.cpp and Frame.h are responsible for data framing which includes adding header byte, end  byte, and byte stuffing to the sending packet.
And the decoding of the receiving packet (base on the header byte and end byte etc).

One thing worth mentioning, the framing is equiped with a CRC check. The transmitted data pack shoulbe match the CRC value so as to consider receiving a complete packet.

