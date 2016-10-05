//
// Copyright (c) 2012 Regents of the SIGNET lab, University of Padova.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the University of Padova (SIGNET lab) nor the 
//    names of its contributors may be used to endorse or promote products 
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

/**
 * @file mfsk_whoi_mm/mserial.h
 * @author Riccardo Masiero, Matteo Petrani
 * \version 1.0.0
 * \brief  Header of the class derived from UWMconnector to handle the RS-232 serial connection between NS-Miracle and WHOI Micro-Modem devices 
 */

#ifndef MSERIAL_H
#define MSERIAL_H

#include <map>
#include <termios.h> // POSIX terminal control definitions
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <assert.h>
#include <map>
//#include "boost/assign.hpp"

// MACRO definition(s). 
#define _POSIX_SOURCE 1 /**< Flag to indicate a POSIX compliant source. */
#define _MODEM_OK 1 /**< Variable to test the right opening of the modem's connection. */
#define _MAX_MSG_LENGTH (0x6000) /**< Variable defining the maximum length of the messages exchanged between host and modem */
#define _MAX_QUEUE_LENGTH 200 /**< Maximum length of queue containing the messages from modem */

using namespace std;

typedef std::map <int, speed_t> BaudMap;

static BaudMap createBaudMap() {
	BaudMap m;
	m[4800] = B4800;
	m[9600] = B9600;
	m[19200] = B19200;
	m[38400] = B38400;
	m[57600] = B57600;
	m[115200] = B115200;
	m[460800] = B460800;
	m[921600] = B921600;
	m[1000000] = B1000000;
	m[2000000] = B2000000;
	m[4000000] = B4000000;
	return m;
}

static const BaudMap BAUD_LIST = createBaudMap();

struct msgModem {
  string msg_rx; /**< Message from the modem.*/
  int msg_length; /**< Length of the message (bytes).*/
};

using namespace std;

// Forward declaration(s)
class Enea_opt_driver;

/**
 * Class used to manage host/modem string exchange via serial connection (this class is a derived class of UWMconnector).
 */
class Mserial
{   
  int fd; /**< Pointer to the serial. */
	int rc; /**< Indicator of the thread creation. */
	struct termios oldtio,newtio; /**< Containers of the serial setups. */
	pthread_t thread_id; /**< Id of a parallel thread. */
	char msg_tx[_MAX_MSG_LENGTH]; /**< Message to be transmitted host to modem. */
	speed_t baudrate; /**< Variable to set the desired transmission baudrate of the serial connection. */
		
	/**
	 *  Set new port settings. Method to set the serial port parameters of the termios struct.
	 *  @param[in] tio_p pointer to a termios struct to modify
	 *  @param[out] newtio (i.e., the member Mserial::newtio)
	 */  
	void new_port_settings(struct termios *tio_p); 

	public:
	
	/** 
	 * Class constructor.
	 * 
	 * @param pmDriver_  pointer to the UWMdriver object to link with this UWMconnector object.
	 * @param pathToDevice_ the path to the device that must be connected with NS-Miracle (e.g., /dev/ttyUSB0 for a serial connection)
	 * @parab baudrate_ variable to set the desired transmission baudrate of the serial connection. 
	 */
	Mserial(Enea_opt_driver*,std::string, int baudrate_);

	/**
	 * Class destructor.
	 */
	virtual ~Mserial();
	
	/**
         * Method to open the connection with the modem. 
	 * 
	 *  @return _MODEM_OK (see uwmconnector.h) if everything went fine, any other int value otherwise.
	 */
	virtual int openConnection();
		
	/**
         * Method to close the connection with the modem. 
	 */ 
	virtual void closeConnection();
		
	/**
         * Method for writing to the modem. 
	 * 
	 * @param[in] str the string to pass to the modem
	 * @return w_bytes, the number of transmitted bytes 
	 */
	int writeToModem(std::string);
	
	/**
	 * Method to read the pointer to the serial where to write (it is needed by the reading function invoked by the parallel thread)
	 * 
	 * @return Mserial::fd
	 */
	int getSerial(){return fd;}

	/**
	 * Method to check the receiving modem buffer.
	 * 
	 * @return return_str the string corresponding to the last received message from the modem
	 */
	std::string readFromModem();
	
	/**
	 * Method to retreive the checking period.
	 * 
	 * @return return_str the string corresponding to the last received message from the modem
	 */
	double getPeriod();

	/**
	 * Method to check the receiving modem buffer.
	 * 
	 * @return Enea_opt_driver::getPeriod()/10, in order to check the serial faster than the modem
	 */
	int inline getDebug() { return debug_; }
	
	queue<msgModem> queueMsg; /**< Queue used to buffer incoming strings from the modem.*/

protected:
	int debug_; /**< Flag to enable debug mode (i.e., printing of debug messages) if set to 1 */
  Enea_opt_driver* pmDriver; /**< Pointer to UWMdriver object that contains this UWMconnector.*/
  std::string pathToDevice; /**< The path to be connected with the modem device */
	        
};


/** 
 * Function to read from the modem via a serial connection (it must be called as a pure C function).
 */
extern "C"
{
	void *read_process_mserial(void *);
}

#endif /* MSERIAL_H */