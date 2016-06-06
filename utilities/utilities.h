//
// Copyright (c) 2016 Regents of the SIGNET lab, University of Padova.
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
 * @file   main.cc
 * @author Filippo Campagnaro
 * @version 1.0.0
 *
 * \brief Haeder of utilities functions
 *
 */

#ifndef T2MO_UTILITIES
#define T2MO_UTILITIES

#include "mdriverS2C_EvoLogics.h"

#define ACK 1
#define NOACK 0
#define BROADCAST_ADD 255

// Used to measure intervals and absolute times
typedef int64_t msec_t;

// Get current time in milliseconds from the Epoch (Unix)
// // or the time the system started (Windows).
msec_t time_ms(void);

/**
 * Build a message of fixed length, composed by a string, a sequence number and 
 * free spaces
 * @param std::string m base of the message
 * @param int c counter of the messages
 * @param int size size in byte of the message.
 *
 * @return the built message
**/
std::string create_message(std::string m, int c, uint size);

/**
 * Gives the number of milliseconds passed from January 1st, 1970 at UTC
 *
 * @return the Unix Time Stamp in milliseconds
**/
unsigned long int getEpoch();

/**
 * Connect the modem socket
 *
 * @param std::string ip Ip address of the modem
 * @param std::string port socket port number 
 * @param int ID Identifier of the node
 * @param bool set_id set the actual id of the node or not (not by default).
 * @return the driver
**/
MdriverS2C_EvoLogics* connectModem(std::string ip, std::string port, int ID, 
											bool set_id = false, std::string log_name = "");

/**
 * Send a packet
 *
 * @param MdriverS2C_EvoLogics* pmDriver pointer to the modem socket
 * @param int dest Destination node Identifier
 * @param std::string message message that has to be sent
 * @param bool ack flag to indicate whether to transmit IM with or without ack request
**/
void transmit(MdriverS2C_EvoLogics* pmDriver, int dest, std::string message, bool ack = false);

/**
 * Send via keep_online mode
 *
 * @param MdriverS2C_EvoLogics* pmDriver pointer to the modem socket
 * @param int dest Destination node Identifier
 * @param std::string message message that has to be sent
 * @param bool ack flag to indicate whether to transmit IM with or without ack request
**/
void transmitBurst(MdriverS2C_EvoLogics* pmDriver, int dest, std::string message);


double tod();

int time2epoch(int hour, int min, int sec);

#endif