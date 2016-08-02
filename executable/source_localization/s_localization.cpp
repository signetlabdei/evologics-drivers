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
 * @file    s_localization.cpp
 * @author  Roberto Francescon
 * @version 0.0.1
 *
 * @brief Implementation of the source localization Matlab interface to be used
 *        in the CMRE Lisbon experiment (July 2016).
 *
 */

#include <s_localization.h>

#include <utilities_ll.h>
#include <sstream>
#include <string>
#include <sstream>
#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

// Check if there something to transmit
bool check_tx_file()
{
  ifstream check_file;
  check_file.open(tx_check_file);
  bool flag = check_file.is_open();
  if (flag)
    {
      check_file.close();
      remove(tx_check_file);
    }
  return flag;
}


// Read what you need to transmit
std::string read_tx_file()
{
  // std::stringstream ss_n;
  // ss_n << pck_n;
  // std::string pck_n_s = ss_n.str();
  std::string filename = string(tx_file); 
  std::ifstream input_file_;
  input_file_.open(filename.c_str());
  std::string message;
  if (input_file_.is_open())
    {
      //std::string message_line;
      std::getline(input_file_, message);
      input_file_.close();
      remove(filename.c_str());
    }
  return message;
}


int main(int argc, char* argv[])
{
  // Check parameters
  cout << "Welcome to Source Localization experiment" << endl;
  if (argc != 4)
    {
      cerr << "Usage:" <<endl;
      cerr << "./s_localization ip nodeID sourceID" << endl;
      return -1;
    }

  // Initialization
  std::string ip = argv[1];
  std::string port = "9500";//argv[2];
  int nodeID = atoi(argv[2]);
  int sourceID = atoi(argv[3]); // ID of the source in the current run
  
  // Set log files and start
  std::ofstream log_stream(0);
  std::ostringstream node_ID;
  node_ID << nodeID;
  std::string logfilename = "./source_loc_log_" + node_ID.str() + ".out";
  log_stream.open(logfilename.c_str(), std::ios_base::app);

  log_stream << "[" << getEpoch()
	     << "]::Beginning of Experiment - Transmissions log"
	     << endl;
  std::string drivers_file = string(drivers_logfile)+"_"+node_ID.str()+".log";

  // Connect to the modem
  MdriverS2C_Evo_lowlev* pmDriver = connectModem(ip, port, 0, drivers_file);
  pmDriver->setSourceLevel(3);
  pmDriver->setPktBitLen(0);
  pmDriver->setBitrate(63);

  cout << "Socket: " << ip <<":" << port << " on Node "<< nodeID << endl;

  std::string msg_file = string(tx_file);
  std::ifstream read_msg;
  std::ifstream check;

  int modemStatus_old = pmDriver->getStatus();
  int modemStatus = pmDriver->updateStatus();

  while (true)
    {
      check.open(tx_check_file);
      if (check.is_open())  // if something to transmit
	{

	  //std::string message = read_tx_file();
          read_msg.open(msg_file.c_str());
	  std::string message;
	  std::getline(read_msg, message);

          if (!message.empty())
	    {

	      transmitRaw(pmDriver, message);
	      log_stream << "[" << getEpoch() << "]::Sent pck: "
			 << message << " | packet_counter = "
			 << tx_pck_cnt << endl;
	      std::cout << "Sent pck: " << message << " | packet_counter = "
			<< tx_pck_cnt << endl;
              tx_pck_cnt++;

	    }
	  read_msg.close();
	  check.close();
	  remove(tx_check_file);
          remove(tx_file);
	}
      else  // then, if something received
        {
          modemStatus_old = pmDriver->getStatus();
          modemStatus = pmDriver->updateStatus();
	  if (modemStatus == MODEM_IDLE_RX && modemStatus_old == MODEM_RX)
	    {      
	      std::string rx_message = pmDriver->getRxPayload();
	      if (rx_message.length() > 4) // exclude noisy chirps
		{
		  cout << "Received: " << rx_message << endl;
		  log_stream << "[" << getEpoch() << "]::Received pck: "
			     << rx_message << " | packet_counter = "
			     << rx_pck_cnt << endl;

		  // Write a file for matlab to read
		  std::stringstream ss;
		  ss << rx_pck_cnt;
		  std::string pck_n_s = ss.str();
		  std::string filename_rx = rx_fileinit+pck_n_s+rx_filextension;
		  std::ofstream output_file_;
		  output_file_.open(filename_rx.c_str(), std::ofstream::app);
		  output_file_ << rx_message << std::endl;
		  output_file_.close();

		  rx_pck_cnt++;
		}
	    }
	}
      usleep(min_sleep_time);
    }


}

