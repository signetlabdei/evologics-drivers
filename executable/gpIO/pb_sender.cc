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
 * @file   pb_sender.cc
 * @author Filippo Campagnaro
 * @version 1.0.0
 *
 * \brief Implementation of simple transmitter that sends monitoring messages
 * about the value obtained by a sensor in the input PIN of a pandaboard.
 */

#include "utilities.h"
#include "GPIO.h"

// Global Declaration:
//GPIO setting
int GPIO_32_status = 0;
const std::string GPIO_32_pin = "32";
const std::string GPIO_32_direction= "in";
//Modem messages
static const std::string MESSAGE_STATUS_OK = "0";
static const std::string MESSAGE_STATUS_ALLARM = "1";
int main(int argc, char* argv[]) {
	//check arguments
	if (argc < 5) {
		cerr << "Insert ID, RECEIVER, ip and port!" << endl;
		return -1;
	}
	//set connection to the acoustic modem
	cout << "Ciao" << endl;
	int ID = atoi(argv[1]);
	int RECEIVER = atoi(argv[2]);
	std::string ip = argv[3];
	std::string port = argv[4];
	cout << "ID " << ID << endl;
	MdriverS2C_EvoLogics* pmDriver = connectModem(ip, port, ID);

	//set connection to pandaboard GPIO
	Init_GPIO(GPIO_32_pin,GPIO_32_direction);
		
	//check sensor and signal
	int ciclo = 1;
	while (true) {
		std::string value = toggle_input(GPIO_32_pin);
		std::string message = (value.substr(0,1).compare("1")==0) ? MESSAGE_STATUS_ALLARM : MESSAGE_STATUS_OK;
		transmit(pmDriver,RECEIVER, message, NOACK);
		cout << "Ciclo" << ciclo << "Inviato " << ID << " to " << RECEIVER << " " << message << endl;
		sleep(1);
		ciclo++;
	}
	return 0;
}
