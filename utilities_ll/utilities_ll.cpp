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
 * @file   utilities_ll.cpp
 * @author Filippo Cmapagnaro, Roberto Francescon
 * @version 0.0.1
 *
 * \brief implementation of utilities functions for low level drivers
 *
 */

#include "utilities_ll.h"
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

std::string create_message(std::string m, int c, uint size)
{
  std::stringstream message;
  message << m;
  message << c;
  if (message.str().size()> size)
   {
    cerr << "Message too long" << endl;
   }
  while (message.str().size() < size) 
  	message << " ";
  return message.str();
}

unsigned long int getEpoch() 
{ 
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (msec_t)tv.tv_sec * 1000 + tv.tv_usec / 1000; 
}

MdriverS2C_Evo_lowlev* connectModem(std::string ip, std::string port,
				   std::string log_name)
{
  MdriverS2C_Evo_lowlev* pmDriver = new MdriverS2C_Evo_lowlev(ip+":"+port);
  // MdriverS2C_EvoLogics* pmDriver = &pmDriver1;
  pmDriver->setLogFile(log_name);
  pmDriver->start();
  usleep(500000);
  // pmDriver->setResetModemQueue(false);
  pmDriver->updateStatus();
  usleep(5000);
  // pmDriver->updateStatus();
  // pmDriver->modemSetID();*/
  // MdriverS2C_EvoLogics *pt = malloc()
  return pmDriver;
}

void transmit(MdriverS2C_Evo_lowlev* pmDriver, std::string message)
{
  pmDriver->updateTx(0xffffffff, message);
  pmDriver->modemTx();
}

void transmitBurst(MdriverS2C_Evo_lowlev* pmDriver, std::string message)
{
  pmDriver->updateTx(0xffffffff, message);
  pmDriver->modemTxBurst();
}

double tod()
{
  timeval tv;
  gettimeofday(&tv, 0);
  double s = tv.tv_sec;
  s += (1e-6 * tv.tv_usec);
  return s;
}

int time2epoch(int hour, int min, int sec)
{
  time_t rawtime;
  time (&rawtime);
  struct tm * timeinfo = localtime (&rawtime);
  timeinfo -> tm_sec = sec;
  timeinfo -> tm_min = min;
  timeinfo -> tm_hour = hour;
  return mktime(timeinfo);
}
