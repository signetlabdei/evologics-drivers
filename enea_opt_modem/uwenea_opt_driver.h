//
// Copyright (c) 2015 Regents of the SIGNET lab, University of Padova.
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

/**
 * @file enea_opt_driver.h
 * @author Filippo Campagnaro
 * \version 1.0.0
 * \brief Header of the class needed by UWMPhy_modem to handle the different transmissions cases and corresponding protocol messages to be generated according to the tcl-user choices and modem firmware, respectively.
 */

#ifndef UWENEA_DRIVER_H
#define UWENEA_DRIVER_H

#include "uwserial.h"
#include "uwinterpreter_opt.h"

#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
//#include "enea_consts.h"

#define MAX_LOG_LEVEL 2

enum MODEM_STATES {
  MODEM_IDLE = 0,
  MODEM_TX,
  MODEM_RX,
  MODEM_CFG_RX,
  MODEM_CFG,
  MODEM_QUIT
};

enum LOG_LEVEL {
  LOG_LEVEL_ERROR = 0,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG
};

typedef enum LOG_LEVEL log_level_t;

typedef enum MODEM_STATES modem_state_t;

using namespace std;

// Forward declaration(s)
class UWEneaMPhy_opt_modem; 
class UWMcodec;

/** 
 * The class needed by UWMPhy_modem to handle the different transmissions cases and corresponding protocol messages to be generated according to the tcl-user choices and modem firmware, respectively. This class contains all the basic functions needed to drive modem in transmitting/receiving messages. NOTE: this class must be inherited and extended to implement any specific modem driver.
 */
class Enea_opt_driver
{
public:

	static std::string int2str(int x) {
		stringstream convert;   // stream used for the conversion
		convert << x; 
		return convert.str();
	};

  /** 
	 * Class constructor.
	 * 
	 * @param pmModem_ pointer to the UWMPhy_modem object to link with this UWMdriver object.
	 */
	Enea_opt_driver(std::string pToDevice_, int serial_baudrate_);

	/**
	 * Class destructor.
	 */
	~Enea_opt_driver();

	/**
	 *  Driver starter. This method starts the driver performing all the needed operations 
	 *  to open an host-modem connection. 
	 */
	virtual void start();

	/**
	 *  Driver stopper. This method should be used before stopping the simulation. It closes and, if needed, 
	 *  resets all the opened files and ports.
	 */
	virtual void stop();


  /**
   * Calculate the epoch of the event. Used in sea-trial mode
   * @return the epoch of the system
   */
  inline unsigned long int getEpoch() {return time(NULL);}

	/**
	 *  Method to notify to the driver that there is a packet to be sent via modem.
	 *  NOTE: when this function is called (by an UWMPhy_modem object), the driver's status must be set to TX_ and the packet must be sent immediately to the modem.
	 */
	virtual void modemTx();

	/**
	 *  Method to set the modem bitrate.
	 *  @param brate transmission and reception bitrate
	 */
	virtual void modemSetBitrate(int brate);

	/**
	 *  Method to set the analog threshold.
	 *  @param threshold analog reception threshold
	 */
	virtual void modemSetThreshold(int threshold);

	/**
	 *  Method to set the transmitting LEDs.
	 *  @param led_flag flag to set which led are used
	 */
	virtual void modemSetLED(int led_flag);

	/**
	 *  Method to set the analog mode.
	 *  @param an_mode flag to set the analog mode
	 */
	virtual void modemSetAnalog(int an_mode);

	/** 
	 *  Method to update modem status. This method has to update the modem status according to the  messages 
	 *  received from the modem/channel (e.g., after a check of the modem buffer's output). NOTE: This method may return after an arbitrary period if nothing has happened, but it must return immediately after a change of UWMdriver::status.
	 * 
	 *  @return UWMdriver::status, the updated modem's status.
	 */
	virtual modem_state_t updateStatus();

	/** 
	 *  Method to reset the modem status. NOTE: this function should be used by the UWMPhy_modem object linked to this
	 *  UWMdriver (the one pointed by pmModem) to reset the status to _IDLE after the processing of a received packet or
	 *  to abort a given ongoing reception process.
	 * 
	 *  @param[out] status  (i.e., the member UWMdriver::status), changed to \e IDLE_.
	 */
	void resetModemStatus();

	/**
	 * Method to update the values of both UWMdriver::payload_tx and UWMdriver::dest. NOTE: This method should be used by an
	 * UWMcodec object linked to this UWMdriver (and contained in the UWMPhy_modem object pointed by pmModem)
	 * 
	 *  @param[in] ptx the payload of the modem packet to transmit acoustically.
	 *  @param[out] payload_tx  (i.e., the member UWMdriver::payload_tx), changed to \e ptx.
	 */
	void updateTx(std::string);

	/**
	 * Method to write in UWMdriver::payload_rx, UWMdriver::src and UWMdriver::dstPktRx. NOTE: This method should be used 
	 * by an UWMinterpreter object linked to this UWMdriver (and contained in the UWMPhy_modem object pointed by pmModem)
	 * 
	 *  @param[in] prx the payload of the last packet acoustically received.
	 *  @param[out] payload_rx  (i.e., the member UWMdriver::payload_rx), changed to \e prx.
	 */
	void updateRx(std::string);

	/**
	 * Method to return the modem's status.
	 * 
	 * @return UWMdriver::status. 
	 */
	modem_state_t getStatus()
	{
	  return status;
	}

	/**
	 * Method to access to the payload of the last packet acoustically received. NOTE: This function should be used by the
	 * UWMcodec object linked to this UWMdriver (and contained in the UWMPhy_modem object pointed by pmModem) to recover the
	 * orginal NS-Miracle to be sent to the above protocol layers of the simulator.
	 * 
	 * @return UWMdriver::payload_rx. 
	 */	 
	std::string getRxPayload()
	{
	  return payload_rx;
	}

	/**
	 * Method to return the flag used to enable debug messages.
	 * 
	 * @return UWMPhy_modem::debug_
	 */
	inline int getDebug() { return debug_; }


	inline void setDebug(int dbg) { debug_ = dbg; }
        
	/**
	 * Method to return the flag used to enable the printing of log messages in UWMPhy_modem::logFile.
	 * 
	 * @return UWMPhy_modem::log_
	 */
	log_level_t getLogLevel();

	void setLogLevel(log_level_t);
	
	/**
	 * Method to return the name of the disk-fiel used to print the log messages.
	 * 
	 * @return UWMPhy_modem::logFile
	 */
	std::string getLogFile();

	void setLogFile(std::string);
	 
/*  virtual void emptyModemQueue() = 0;*/
	 
	void printOnLog(log_level_t log_level,string module, string message);

	/**
   * Return the MAC id of the node
   * @return the MAC address of the node
   */
	int getID();

	void setID(int id);

	/**
   * Return the modem checking period
   * @return pmModem->getPeriod()
   */
	double getPeriod();

	void setPeriod(double period);

/*	void checkConf();

	void checkTx();*/

	/**
   * Check if a packet has been received
   * @param string rx_msg: the message received from the optical modem
   */
	void checkRx(std::string rx_msg);

protected:

	MinterpreterOp pmInterpreter; /**< pointer to the object that builds/parses the necessary messages to make
	                                UWMdriver able to communicate with a real modem */
	Mserial pmConnector; /**< pointer to the object that handles the physical transmission and
	                                      reception of acoustic packets */

	modem_state_t status; /**< Status of the driver's general state machine. Seven possible statuses = \e _IDLE, \e _TX, \e _RX , \e _IDLE_RX,\e _CFG, \e _TX_PAUSED and \e _TX_RX.*/

	// TX VARIABLES (variables for the next packet to be transmitted)
	std::string payload_tx; /**< String where to save the payload of the next packet to send via modem. NOTE: an object of the class UWMcodec must write here after the host-to-modem mapping. */
	
	// RX VARIABLES (variables reffering to the last received packet)
	std::string payload_rx; /**< String where to save the payload of the last packet received via modem. NOTE: an object of the class UWMinterpreter must write here after a the parsing of a received data packet; instead, an object of the class UWMcodec reads here before the modem-to-host mapping. */
	int debug_; /**< Flag to enable debug mode (i.e., printing of debug messages) if set to 1 */
	std::ofstream outLog; /**< output strem to print into a disk-file log messages. See UWMPhy_modem::logFile.*/
	int id_;
	double period_;
	string log_file_;
	log_level_t log_level_;
	/** 
	 * Link connector. This method must be used by any derived class D of UWDriver to link the members pmInterpreter and pmConnector of UWMdriver to the corresponding derived objects contained in D.
	 * @see: e.g. MdriverS2C_EvoLogics
	 *
	 * @param[in] pmInterpreter_ pointer to a UWMinterpreter object
	 * @param[in] pmConnector_ pointer to an UWMconnector object
	 * @param[out] pmInterpreter (i.e., the member UWMPhy_modem::pmInterpreter)
	 * @param[out] pmConnector (i.e., the member UWMPhy_modem::pmConnector)
	//  */
	// void setConnections(MinterpreterOp*, Mserial*);

	/** 
	 * Method to manage modem to host and host to modem communications. This method has to handle the different transmissions cases and corresponding protocol messages to be generated according to the tcl-user choices and modem firmware, respectively.
	 */
/*	virtual void modemTxManager() = 0;*/
        
};
#endif	/* UWMDRIVER_H */
