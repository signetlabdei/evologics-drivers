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
 * @file  burst_receiver.cc
 * @author Filippo Campagnaro
 * @version 1.0.0
 *
 * \brief Implementation of simple receiver 
 */

#include "utilities_ll.h"

int main(int argc, char* argv[])
{
  cout << "Hello!" << endl;
  if (argc < 3)
    {
      cerr << "Usage: ./burstll_rx ip port msg_bit_len" << endl;
      return -1;
    }
  //set connection to the acoustic modem
  std::string ip = argv[1];
  std::string port = argv[2];
  int bitlen = atoi(argv[3]);
  MdriverS2C_Evo_lowlev* pmDriver = connectModem(ip, port, bitlen,
                                                 "receiver_lowlevel.log");
  int modemStatus_old = pmDriver->getStatus();
  int modemStatus = pmDriver->updateStatus();

  while (true)
    {
      
      if (modemStatus == MODEM_IDLE_RX && modemStatus_old == MODEM_RX)
      {
	  std::string rxMessage = pmDriver->getRxPayload();
	  cout << "Received: " << rxMessage << endl;
          //pmDriver->resetModemStatus();
       }
      usleep(500000);			
      modemStatus_old = pmDriver->getStatus();
      modemStatus = pmDriver->updateStatus();
    }

  // Turn off DSP
  pmDriver->stop();

  return 0;
}
