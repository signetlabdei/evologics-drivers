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
 * @file   burst_transmitter_ll.cc
 * @author Filippo Campagnaro, Roberto Francescon
 * @version 0.0.1
 *
 * \brief Implementation of simple transmitter 
 *
 */
#include "utilities_ll.h"
/**
 * Main: the transmitter
**/
int main(int argc, char* argv[])
{
  // CHECK parameters
  cout << "Welcome!" << endl;
  if (argc < 5)
    {
      cerr << "Usage:" <<endl;
      cerr << "./mycc ip port period[ms] packet_in_a_burst" << endl;
      return -1;
    }
  // INITIALIZATION
  std::string ip = argv[1];
  std::string port = argv[2];
  double period = atof(argv[3]) * pow10(3);
  int packet_in_a_burst = atoi(argv[4]);
  if ( !packet_in_a_burst )
  {
    cerr << "Error: packet_in_a_burst must be > 0" <<endl;
    return -1;
  }
  uint const message_length = 18;
  int backoff_period = 10 * pow10(6);
  int packet_counter = 0;
  std::string base_message = "CIAO CIAO";
  
  // SET LOG FILES
  std::stringstream transm_file;
  std::ofstream transm_file_stats(0);
  transm_file << "./senderll_log.out";
  transm_file_stats.open(transm_file.str().c_str(),std::ios_base::app);
  transm_file_stats << "[" << getEpoch()
		    << "]:: Beginning of Experiment - Tranmsmissions log"
		    << endl;

  // CONNECT to the modem
  MdriverS2C_Evo_lowlev* pmDriver = connectModem(ip, port,
                                                 base_message.length()*8,
						 "sender_lowlevel.log");
  int modemStatus_old = pmDriver->getStatus();
  int modemStatus = pmDriver->updateStatus();

  cout << "Socket: " << ip <<":" << port << endl;
  // START THE APPLICATION
  while (packet_counter < packet_in_a_burst)
   {
    usleep(period);
    // SEND a packet of the "burst" (in IM mode)
    std::string complete_message = create_message(base_message, packet_counter,
                                                  message_length);

    transmit(pmDriver, complete_message);
    transm_file_stats << "[" << getEpoch() << "]:: Sent pck: "
		      << complete_message << "packet_counter = "
                      << packet_counter << endl;
    std::cout << "[" << getEpoch() << "]:: Sent pck: "
	      << complete_message << "packet_counter = "
              << packet_counter << endl;
    packet_counter ++;
    pmDriver->updateStatus();
   }
  // STOP DSP
  pmDriver->stop();
  return 0;
}

