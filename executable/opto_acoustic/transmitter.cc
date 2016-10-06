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
 * @file   burst_transmitter.cc
 * @author Filippo Campagnaro
 * @version 1.0.0
 *
 * \brief Implementation of simple transmitter 
 *
 */
#include "uwenea_opt_driver.h"
#include "utilities.h"
/**
 * Main: the transmitter
**/
int main(int argc, char* argv[]) {
	// CHECK parameters
  cout << "Ciao" << endl;
	if (argc < 9) {
		cerr << "Usage:" <<endl;
   	cerr << "./mycc ID ID_RECEIVER serial baud ip port period[ms] packet_in_a_burst" << endl;
		return -1;
	}
  // INITIALIZATION
 	int ID = atoi(argv[1]);
 	int ID_RECEIVER = atoi(argv[2]);
	std::string serial = argv[3];
	int baud = atoi(argv[4]);
	std::string ip = argv[5];
	std::string port = argv[6];
	double period = atof(argv[7]) * pow10(3);
	int packet_in_a_burst = atoi(argv[8]);
	if ( !packet_in_a_burst ) {
		cerr << "Error: packet_in_a_burst must be > 0" <<endl;
		return -1;
	}
	Enea_opt_driver optDriver(serial, baud);
	optDriver.setID(ID);
  optDriver.setPeriod(0.01);
	optDriver.start();

	MdriverS2C_EvoLogics* acDriver = connectModem(ip, port, ID_RECEIVER);
	int ac_modemStatus_old = acDriver->getStatus();
	int ac_modemStatus = acDriver->updateStatus();

 	int backoff_period = 10 * pow10(6);
	int packet_counter = 0;
	std::string complete_message = "CIAO CIAO";

  
  // SET LOG FILES
	std::stringstream transm_file;
	std::ofstream transm_file_stats(0);
  transm_file << "./sender_log.out";
  transm_file_stats.open(transm_file.str().c_str(),std::ios_base::app);
  transm_file_stats << "[" << optDriver.getEpoch() << "]:: Beginning of )Experiment - Tranmsmissions log" << endl;
	std::string rxMessage = "";
  // START THE APPLICATION
	while (packet_counter < packet_in_a_burst) {
    // SEND a packet of the "burst" (in IM mode)
		std::stringstream converter;
		converter << packet_counter;
		complete_message = converter.str();
    optDriver.updateTx(complete_message);
	 	optDriver.modemTx();
	 	if (ac_modemStatus == MODEM_IDLE_RX && ac_modemStatus_old == MODEM_RX) {
			rxMessage = acDriver->getRxPayload();
			cout << "Ricevuto from acoustic " << ID_RECEIVER << " " << rxMessage << endl;
			usleep(period);
			optDriver.updateTx(rxMessage);
	 		optDriver.modemTx();
	 		packet_counter = atoi(rxMessage.c_str());
		}
		else {
			packet_counter ++;
		}
		cout << "Inviato from " << ID << " pachet num " << packet_counter 
         << " payload "  << complete_message << endl;
		usleep(period);
		ac_modemStatus_old = acDriver->getStatus();
		ac_modemStatus = acDriver->updateStatus();
	}
	return 0;
}
