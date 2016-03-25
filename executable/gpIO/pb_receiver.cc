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
 * @file   pb_receiver.cc
 * @author Filippo Campagnaro
 * @version 1.0.0
 *
 * \brief Implementation of simple receiver that uses two LEDs to 
 * signal the status of a remote system, via pandaboard PINs.
 */

#include "utilities.h"
#include "GPIO.h"

// Global Declaration:
//GPIO setting
int GPIO_status = 0;
const int LOW = 0;
const int HIGH = 1;
const std::string GPIO_RED_pin = "32";
const std::string GPIO_GREEN_pin = "33";
const std::string GPIO_direction= "out";

const std::string ALLARM_Message= "1";

int main(int argc, char* argv[]) {
	cout << "Ciao Stazione" << endl;
	if (argc < 5) {
		cerr << "Insert ID, RECEIVER, ip and port!" << endl;
		return -1;
	}
	//set connection to the acoustic modem
	int ID = atoi(argv[1]);
	int RECEIVER = atoi(argv[2]);
	std::string ip = argv[3];
	std::string port = argv[4];
	cout << "ID " << ID << endl;
	MdriverS2C_EvoLogics* pmDriver = connectModem(ip, port, ID);
	int modemStatus_old = pmDriver->getStatus();
	int modemStatus = pmDriver->updateStatus();

	//set connection to pandaboard GPIO
	Init_GPIO(GPIO_RED_pin,GPIO_direction);
	Init_GPIO(GPIO_GREEN_pin,GPIO_direction);
	std::string led = GPIO_GREEN_pin;
	while (true) {
		if (modemStatus == MODEM_IDLE_RX && modemStatus_old == MODEM_RX) {
			std::string rxMessage = pmDriver->getRxPayload();
			cout << "Ricevuto " << ID << " " << rxMessage << endl;
			if (!(abs(rxMessage.substr(0,1).compare(ALLARM_Message)) + abs(led.compare(GPIO_GREEN_pin)))) {
				toggle_output(led, LOW);
				led = GPIO_RED_pin;
				toggle_output(led, HIGH);
				GPIO_status = HIGH;
				cout << "ALLARM" << endl;
			}
			else if (!(abs(led.compare(GPIO_RED_pin)) + !(rxMessage.substr(0,1).compare(ALLARM_Message)))) {
				toggle_output(led, LOW);
				led = GPIO_GREEN_pin;
				toggle_output(led, HIGH);
				GPIO_status = HIGH;
			}
		}
		toggle_output(led, GPIO_status);
		GPIO_status = ! GPIO_status;
		sleep(1);			
		modemStatus_old = pmDriver->getStatus();
		modemStatus = pmDriver->updateStatus();
	}
	return 0;
}
