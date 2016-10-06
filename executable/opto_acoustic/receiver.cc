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

#include "uwenea_opt_driver.h"
#include "utilities.h"

int main(int argc, char* argv[]) {
	cout << "Ciao Stazione" << endl;
	if (argc < 7) {
		cerr << "Insert ID ID_TRANSMITTER serial baud ip port" << endl;
		return -1;
	}
	//set connection to the acoustic modem
	int ID = atoi(argv[1]);
	int ID_TRANSITTER = atoi(argv[2]);
	std::string serial = argv[3];
	int baud = atoi(argv[4]);
	std::string ip = argv[5];
	std::string port = argv[6];
	Enea_opt_driver optDriver(serial, baud);
	cout << "ID " << ID << endl;
	optDriver.setID(ID);
  optDriver.setPeriod(0.01);
	optDriver.start();
	int opt_modemStatus_old = optDriver.getStatus();
	int opt_modemStatus = optDriver.updateStatus();

	MdriverS2C_EvoLogics* acDriver = connectModem(ip, port, ID);
	int ac_modemStatus_old = acDriver->getStatus();
	int ac_modemStatus = acDriver->updateStatus();
	std::string rxMessage = "";
	int packet_counter = 0;

	while (true) {
		if (opt_modemStatus == OPT_MODEM_IDLE && opt_modemStatus_old == OPT_MODEM_RX) {
			rxMessage = optDriver.getRxPayload();
			cout << "Ricevuto from optical " << ID << " " << rxMessage << endl;
			if( atoi(rxMessage.c_str()) == packet_counter ){
				packet_counter++;
			}
			else if( atoi(rxMessage.c_str()) > packet_counter ) {
				std::stringstream converter;
				converter << packet_counter;
				transmit(acDriver, ID, converter.str(), NOACK);
			}
		}
		usleep(10);			
		opt_modemStatus_old = optDriver.getStatus();
		opt_modemStatus = optDriver.updateStatus();

	}
	return 0;
}
