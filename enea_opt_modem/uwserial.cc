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
 * @file mfsk_whoi_mm/mserial.cc
 * @author Riccardo Masiero, Matteo Petrani
 * \version 1.0.0
 * \brief  Implementation of the Mserial class. 
 */

#include "uwserial.h"
#include "uwenea_opt_driver.h"
#include <algorithm>    // std::find

Mserial::Mserial(Enea_opt_driver* pmDriver_, std::string pathToDevice_, int baudrate_){
  // Variable initialization
  fd = 0;
  rc = 0;
/*  const speed_t *start_baud = BAUD_LIST;
  const speed_t *end_baud = BAUD_LIST + sizeof(BAUD_LIST)/sizeof(*BAUD_LIST);
  cout << start_baud << " " << baudrate_ << " " << end_baud << "\n";*/
  /*assert((int) (std::find(start_baud, end_baud, baudrate_) != end_baud));*/
  assert(BAUD_LIST.find(baudrate_)!=BAUD_LIST.end());
  baudrate = BAUD_LIST.find(baudrate_)->second;
  pmDriver = pmDriver_;
  pathToDevice = pathToDevice_;
  debug_ = 0; // intit when opening, now driver pointer still in initialization
  if (debug_) {
  	cout << this << ": in constructor of Mserial which points to driver: " << pmDriver << "\n";
  }
}

Mserial::~Mserial(){
}
	
int Mserial::openConnection(){
  debug_ = pmDriver -> getDebug();
	if (debug_) {cout << this << ": in openConnection() method of Mserial. Note: ID = " <<  pmDriver -> getID()  << "\n";}       
        // Open modem device for reading and writing and not as controlling tty because we don't want to get killed if linenoise sends CTRL-C.
	// Opening.
  fd = open(pathToDevice.c_str(), O_RDWR | O_NOCTTY);
	// Check for opening errors.
	if (fd <0) {
		perror(pathToDevice.c_str()); 
		exit(1);
	}
	usleep(1500000);
  // Save current serial port settings.
	tcgetattr(fd,&oldtio);
	// Set the new port settings.
	
 /*tcsetattr(fd, TCSANOW, &toptions);*/

	new_port_settings(&newtio);
	newtio.c_cflag &= ~PARENB;
  newtio.c_cflag &= ~CSTOPB;
  newtio.c_cflag &= ~CSIZE;
  newtio.c_cflag &= ~CRTSCTS;
  newtio.c_cflag |= CS8;
  newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  /* commit the serial port settings */
	// Clean the modem line.
	tcflush(fd, TCIFLUSH);
	// Activate the new settings for the port.
	tcsetattr(fd,TCSANOW,&newtio);
	
	// Create the parallel process to read from the serial.
	rc = pthread_create(&thread_id, NULL, read_process_mserial, (void *) this);
	// Check the correct starting of the reading process
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}	
	
	return _OPT_MODEM_OK;
}
		
void Mserial::closeConnection(){
        
	// Restore the old port settings before closing. 
	tcsetattr(fd,TCSANOW,&oldtio);
}
		
int Mserial::writeToModem(std::string str){
  
  if (debug_) {
  	cout << ": in Mserial::writeToModem(string). Note: ID = " 
  		 	 <<  pmDriver -> getID() << ", TX msg = " << str << ", str size = "
  		 	 << str.size() << "\n";
  }

  // Copy the input string in Mserial::msg_tx
  bzero(msg_tx,_OPT_MAX_MSG_LENGTH);
	int msg_ssz = str.size();	
  memcpy(msg_tx, str.c_str(), msg_ssz);
	// Length of the message to be transmitted
	
	// Append to the message that we want to send the windows-complaint string terminator (WHOI Micro-Modem requirements) 
/*	msg_tx[msg_ssz++] = '\r';*/
	msg_tx[msg_ssz++] = '\n';
/*	msg_tx[msg_ssz] = '\0';*/

  if (debug_) {
		cout << ": in Mserial::writeToModem(string). Note: ID = " 
		 	   <<  pmDriver -> getID() << ", msg_tx = " << msg_tx 
		 	   << ", msg_ssz = " << msg_ssz << "\n";
  }
        
	// Send message host to modem via serial
	int w_bytes = write(fd,msg_tx,msg_ssz); 
	
	// Return number of written bytes
	return w_bytes;
}

void Mserial::new_port_settings(struct termios *tio_p){
  
  // Clear struct for new port settings. 
	memset(tio_p, 0, sizeof(*tio_p)); 

  /* baudrate : Set bps rate. You could also use cfsetispeed and cfsetospeed.
   * CRTSCTS : output hardware flow control (only used if the cable has all necessary lines. See sect. 7 of Serial-HOWTO) not used for WHOI micromodems since they want all the flow controls disabled (comment by RM)
   * CS8 : 8n1 (8bit,no parity,1 stopbit)
   * CLOCAL  : local connection, no modem contol
   * CREAD   : enable receiving characters
   */
	(*tio_p).c_cflag = baudrate | CS8 | CLOCAL | CREAD;

	/* IGNPAR  : ignore bytes with parity errors
	 * ICRNL   : map CR to NL (otherwise a CR input on the other computer will not terminate input) - NOTE (by RM) if enable caused the messages from WHOI to be received in two sentences, the actual message and a blank one made only of a CR (or NL) caracter ...  
   */
	(*tio_p).c_iflag = IGNPAR;

	// Raw output.
	(*tio_p).c_oflag = 0;

	/* ICANON : enable canonical input disable all echo functionality, and don't send signals to calling program
   */
	(*tio_p).c_lflag = ICANON;

	// Initialize all control characters default values can be found in /usr/include/termios.h, and are given in the comments, but we don't need them here
	(*tio_p).c_cc[VINTR]    = 0;     // Ctrl-c
	(*tio_p).c_cc[VQUIT]    = 0;     // Ctrl-'\'
	(*tio_p).c_cc[VERASE]   = 0;     // del
	(*tio_p).c_cc[VKILL]    = 0;     // @
	(*tio_p).c_cc[VEOF]     = 4;     // Ctrl-d
	(*tio_p).c_cc[VTIME]    = 0;     // inter-character timer unused
	(*tio_p).c_cc[VMIN]     = 1;     // blocking read until 1 character arrives
	(*tio_p).c_cc[VSWTC]    = 0;     // '\0'
	(*tio_p).c_cc[VSTART]   = 0;     // Ctrl-q
	(*tio_p).c_cc[VSTOP]    = 0;     // Ctrl-s 
	(*tio_p).c_cc[VSUSP]    = 0;     // Ctrl-z 
	(*tio_p).c_cc[VEOL]     = 0;     // '\0' 
	(*tio_p).c_cc[VREPRINT] = 0;     // Ctrl-r 
	(*tio_p).c_cc[VDISCARD] = 0;     // Ctrl-u 
	(*tio_p).c_cc[VWERASE]  = 0;     // Ctrl-w 
	(*tio_p).c_cc[VLNEXT]   = 0;     // Ctrl-v 
	(*tio_p).c_cc[VEOL2]    = 0;     // '\0' 
}

std::string Mserial::readFromModem() {
  
  std::string return_str;
  msgOptModem tmp_;
  if (!queueMsg.empty()){
    tmp_ = queueMsg.front();
    return_str = tmp_.msg_rx;
    queueMsg.pop();
  }else {
    return_str = "";
  }

  std::string parser("\n");

  size_t p_parser = return_str.find(parser);
  
  while (!queueMsg.empty() /*&& ! p_parser == return_str.size() -1*/)
  {
    tmp_ = queueMsg.front();
    return_str += tmp_.msg_rx;
    queueMsg.pop();
    p_parser = return_str.find(parser);
    usleep(1000);
    if (getDebug()) {
			cout << "Mserial::readFromModem() cicla " << "\n";
		}
  }
/*	while (p_parser == std::string::npos)
  {
  	if(!queueMsg.empty()) {
	    tmp_ = queueMsg.front();
	    return_str += tmp_.msg_rx;
	    queueMsg.pop();
	    p_parser = return_str.find(parser);
	    if (getDebug()) {
				cout << NOW << "Mserial::readFromModem() cicla " << "\n";
			}
		}
    usleep(1000);
  }*/
 
  if (getDebug()) {
		cout << "Mserial::readFromModem() = " <<  return_str  
				 << ", queueMsg.size() = " << queueMsg.size() << "\n";
	}  
  return return_str;	 
}

double Mserial::getPeriod() {
  return pmDriver -> getPeriod()/10;
}

void *read_process_mserial(void *pMserial_me_)
{    
  // Array to store the received message
	char msg_rx[_OPT_MAX_MSG_LENGTH + 1];
	msgOptModem tmp_;
	// Output stream to write to pMserial_me->getReadingBuff()
	std::ofstream out;
	
	// Cast to the Mserial pointer passed as argument
	Mserial* pMserial_me = (Mserial*) pMserial_me_;
	
	if (pMserial_me->getDebug()) {
		cout << "      : in the  read_process() method (parallel thread). Note: readFromModem = " 
		<<  pMserial_me->readFromModem()  << "\n";
	}  
 
	while (1) 
	{
		// Read from the serial
		tmp_.msg_length = read(pMserial_me->getSerial(),msg_rx,_OPT_MAX_MSG_LENGTH);
		// Set end of string
		if( tmp_.msg_length < 0 ) {
        perror( "SERIAL::READ::ERROR_READ_FROM_SERIAL" );
    }
		msg_rx[tmp_.msg_length]='\0';
		/*
		for (int i = 0; i < tmp_.msg_length; i++)
			cout << (unsigned short) (msg_rx[i]) << endl;*/

		if (pMserial_me->queueMsg.size() > _OPT_MAX_QUEUE_LENGTH) {
      cout << "MSERIAL::READ::ERROR::BUFFER_FULL ---> drop the oldest packet" << endl;
			pMserial_me->queueMsg.pop();
		}

		// Write received message into the disk-file readingBuff.
/*		out.open(((pMserial_me->readFromModem()).c_str()), ios::app);
		out << msg_rx;
		out.close();*/

		tmp_.msg_rx.assign(msg_rx, tmp_.msg_length);
		pMserial_me->queueMsg.push(tmp_);
		
		if (pMserial_me->getDebug()) {
			cout << "  : in the  read_process() method (parallel thread). Note:  readFromModem = " 
			<<  tmp_.msg_rx << ", RX message = " <<  msg_rx  << " size = " << "\n";
		}
		/*usleep( pMserial_me->getPeriod()/10 );*/
		usleep(2);
	}
	// Exit the parallel reading thread
	pthread_exit(NULL);	
}
