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
 * @file  m2mo.cc
 * @author Filippo Campagnaro
 * @version 1.0.0
 *
 * \brief Program matlab to modem. Sends the packet incoming from a buffer 
 * through Evologics modems and store incomung packet in another bugger
 */

#include "utilities.h"
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

static char const* check_tx_log_ = "/MacTx.txt";
static char const* tx_log_ = "/FromMac2Phy.txt";

static char const* check_rx_log_ = "/PhyRx";
//static char const* rx_log_ = "'PhyRx*.txt";
static char const* rx_log_ = "/FromPhy2Mac";
static char const* extension_ = ".txt";

static char const token_separator_ = ',';
//check if the transmitting file exists
bool checkTxLog(char * folder) {
	ifstream input_file_;
	string file_name = string(folder) + string(check_tx_log_);
	input_file_.open(file_name.c_str());
	bool transmitting_flag = input_file_.is_open();
	if (!transmitting_flag) { input_file_.close(); }
	return transmitting_flag;
}

//Retreive sending time and data to send
std::string readMessage(char * folder) {
	std::string message_line = "";
	if (!checkTxLog(folder))
		return message_line;
	ifstream input_file_;
	string file_name = string(folder) + string(tx_log_);
	input_file_.open(file_name.c_str());
	if ( !input_file_.is_open() ) 
		return message_line;
	std::getline(input_file_, message_line);
	remove(file_name.c_str());
	file_name = string(folder) + string(check_tx_log_);
	remove(file_name.c_str());
	return message_line;
}

//DONE: print in the received log file and create the flag file for the reception
void printToFile(std::string rx_msg, int counter, char * folder) { 
	char log_name [100];
	sprintf(log_name, "%s%s%d%s", folder, rx_log_, counter, extension_);
	std::ofstream out_file_stats;
	out_file_stats.open(log_name, std::ofstream::out); //erase and create a new file
	out_file_stats << rx_msg << std::endl;
	out_file_stats.close();
	sprintf(log_name, "%s%s%d%s", folder, check_rx_log_, counter, extension_);
	out_file_stats.open(log_name, std::ofstream::out); //erase and create a new file
	out_file_stats.close();
}

double tod() {
	timeval tv;
	gettimeofday(&tv, 0);
	double s = tv.tv_sec;
	s += (1e-6 * tv.tv_usec);
	return s;
}

int time2epoch(int hour, int min, int sec) {
	time_t rawtime;
	time (&rawtime);
	struct tm * timeinfo = localtime (&rawtime);
	timeinfo -> tm_sec = sec;
	timeinfo -> tm_min = min;
	timeinfo -> tm_hour = hour;
	return mktime(timeinfo);
}

//DONE: retreive how long we need to wait before to tx the message, parsing the
//      line imported from the TxLog file and subtracting the current time

long parseWait(std::string message2parse) {
	short hour = 0;
	short min = 0;
	float sec = 0.0;
	::std::stringstream line_stream(message2parse);
	std::string result_ = "";
	std::getline(line_stream, result_, token_separator_);
	hour = atoi(result_.c_str());
	std::getline(line_stream, result_, token_separator_);
	min = atoi(result_.c_str());
	std::getline(line_stream, result_, token_separator_);
	sec = atof(result_.c_str());

	return 1e6 * (time2epoch(hour,min,trunc(sec)) - tod() + sec - trunc(sec));
}

//DONE: retreive the message tx, parsing the line imported from the TxLog file
std::string getMessage(std::string message2parse) {
	std::string message = "";
	::std::stringstream line_stream(message2parse);
	for (int i = 0; i < 3 && std::getline(line_stream, message, token_separator_); i++);
	std::getline(line_stream, message, token_separator_);
	return message;
}

int main(int argc, char* argv[]) {
	cout << "Ciao" << endl;
	if (argc != 4) {
		cerr << "Usage:" <<endl;
    cerr << "./m2mo ID ip port" << endl;
		return -1;
	}
	int ID = atoi(argv[1]);
	char* dir_label = argv[1];
	std::string ip = argv[2];
	std::string port = argv[3];
	int RECEIVER = 255; // broadcast
	int rx_counter(0);
	cout << "ID " << ID << endl;
	const int dir_err = mkdir(dir_label, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	MdriverS2C_EvoLogics* pmDriver = connectModem(ip, port, ID);
	int modemStatus_old = pmDriver->getStatus();
	int modemStatus = pmDriver->updateStatus();
	const int min_sleeping_time = 5; // 200 ms 
	//main cicle

	while (true) {
	//check if there's something to tx
		bool tx_flag = checkTxLog(dir_label);
		//cout << "flag = " << tx_flag << endl;
		if (tx_flag) {
			//retreive sending time and data to send
			std::string message2parse = readMessage(dir_label);
			cout << getEpoch() << " message2parse = " << message2parse << endl;
			cout << getEpoch() << " getMessage(message2parse)" << getMessage(message2parse) <<endl;
			cout << getEpoch() << " parseWait(message2parse) = " << parseWait(message2parse) << endl;
			usleep (parseWait(message2parse)); //sleep(); if sec.
			transmit(pmDriver,RECEIVER, getMessage(message2parse),NOACK);
			std::cout << getEpoch() << " Sent " << ID << " to " << RECEIVER << " " << getMessage(message2parse) << endl;
		}
		else {
			//check if there is something received and write it in the log
			if (modemStatus == MODEM_IDLE_RX && modemStatus_old == MODEM_RX) {
				std::string rx_msg = pmDriver->getRxPayload();
				rx_counter ++;
				printToFile(rx_msg,rx_counter,dir_label);
				cout << getEpoch() << " Rx Message " << ID << " " << rx_msg << endl;
				pmDriver -> resetModemStatus();
			}
			modemStatus_old = pmDriver->getStatus();
			modemStatus = pmDriver->updateStatus();

		}
		usleep(min_sleeping_time);

	}
	return 0;
}


