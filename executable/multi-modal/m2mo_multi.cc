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
 * @file   m2mo_multi.cc
 * @author Filippo Campagnaro
 * @version 1.0.0
 *
 * \brief Implementation of multimodal matlab to modem 
 *
 */
#include "utilities.h"
#include "m2mo_multi.h"
#include <queue>

bool checkTxLog(char * folder) {
	ifstream input_file_;
	string file_name = string(folder) + string(check_tx_log_);
	input_file_.open(file_name.c_str());
	bool transmitting_flag = input_file_.is_open();
	if (!transmitting_flag) { input_file_.close(); }
	return transmitting_flag;
}

//Retreive sending time and data to send
std::queue<std::string>* readMessages(char * folder) {
	std::string message_line = "";
	std::queue<std::string> *messages_queue = new std::queue<std::string>;
	if (!checkTxLog(folder))
		return messages_queue;
	ifstream input_file_;
	string file_name = string(folder) + string(tx_log_);
	input_file_.open(file_name.c_str());
	if ( !input_file_.is_open() ) 
		return messages_queue;
	while (std::getline(input_file_, message_line))
	{
		messages_queue->push(message_line);
	}
	remove(file_name.c_str());
	file_name = string(folder) + string(check_tx_log_);
	remove(file_name.c_str());
	return messages_queue;
}

//DONE: retreive how long we need to wait before to tx the message, parsing the
//      line imported from the TxLog file and subtracting the current time

msg2send_str* parseMessage(std::string message2parse) {
	msg2send_str* hdr = new msg2send_str();
	::std::stringstream line_stream(message2parse);
	std::string result_ = "";
	std::getline(line_stream, result_, token_separator_);
	hdr->msg_id = atoi(result_.c_str());
	std::getline(line_stream, result_, token_separator_);
	hdr->des_id = atoi(result_.c_str());
	std::getline(line_stream, result_, token_separator_);
	hdr->tec_id = atoi(result_.c_str());
	std::getline(line_stream, result_, token_separator_);
	hdr->data = result_;

	return hdr;
}

//DONE: print in the received log file and create the flag file for the reception
void printToRxFile(int src, int tec_id, int counter, std::string rx_msg, char * folder) { 
	char log_name [100];
	sprintf(log_name, "%s%s%d%s%d%s%s", folder, rx_log_header, tec_id, "_", counter, rx_log_tail, extension_);
	/*std::cout << log_name << endl;*/
	std::ofstream rx_file_report;
	rx_file_report.open(log_name, std::ofstream::out); //erase and create a new file
	rx_file_report << src << "," << rx_msg << std::endl;
	rx_file_report.close();
	sprintf(log_name, "%s%s%d%s%d%s", folder, check_rx_log_, tec_id, "_", counter, extension_);
	rx_file_report.open(log_name, std::ofstream::out); //erase and create a new file
	rx_file_report.close();
}

void reportAck(int msg_id, int tec_id, bool confirmed, char * folder) {
	char ack_log_name [100];
	sprintf(ack_log_name, "%s%s%d%s%d%s%d%s", folder, ack_log_header, msg_id, "_", tec_id, "_", 
		(int) confirmed, extension_);
	std::ofstream ack_file_report;
	ack_file_report.open(ack_log_name, std::ofstream::out); //erase and create a new file
	ack_file_report.close();
}


/**
 * Main: the transmitter
**/
 //TODO: decide how to set number and identifier of phys and their sockets
int main(int argc, char* argv[]) {
	// CHECK parameters
  cout << "Ciao" << endl;
	if (argc < 4) {
		cerr << "Usage:" <<endl;
    cerr << "./mycc ID keep-online ip1 <ip2> <ip3> " << endl;
		return -1;
	}
  // INITIALIZATION
  int ID = atoi(argv[1]);
	bool k_o = bool(atoi(argv[2]));
	char* dir_label = ".";
	std::string port = "9200";
	bool set_id = true;
  // CONNECT to the modem
  /*std::map<int,MdriverS2C_EvoLogics*> pmDriver;*/
	DriversMap pmDriver;
	for (int i = 3; i < argc; i++) {
		if (*argv[i] != '0') {
			cout << getEpoch() << " " << sizeof(argv[i])/sizeof('0') << "_ID:" << ID << "_socket:" << argv[i] <<":" << port;
			pmDriver[i-2] = connectModem(argv[i], port, ID, set_id, argv[i]);
			cout << "connected" << endl;
		}
	}
  std::map<int,int> phy_pending_msg;
	sleep(1);
  
  // RESET THE MODEM AFTER THE INITIALIZATION
  int modemStatus_old;
	int modemStatus;
	for(DriversMap::iterator driver_iter = pmDriver.begin();
    driver_iter != pmDriver.end();
    driver_iter++) {
		modemStatus_old = driver_iter->second->getStatus();
		modemStatus = driver_iter->second->updateStatus();
		driver_iter-> second -> resetModemStatus();
	}

	// SET LOG FILES
	std::ofstream transm_file_stats(0);
	std::ofstream recv_file_stats(0);
	transm_file_stats.open("./sender_log.out", std::ofstream::out | std::ofstream::app); //append or and create a new file
	recv_file_stats.open("./recv_log.out", std::ofstream::out | std::ofstream::app); //append or and create a new file
  transm_file_stats << "./sender_log.out" << endl;;
  recv_file_stats << "./recv_log.out" << endl;
  // START THE APPLICATION
	//main cicle
	std::queue<std::string> *messages_buffer;
	std::string message2parse = "";
	int rx_counter = 0;

	while (true) {
		bool tx_flag = checkTxLog(dir_label);

		if (tx_flag) {
			cout << "something to tx" << endl;
			//retreive sending data to send
			messages_buffer = readMessages(dir_label);
			while (!messages_buffer->empty()) {
				message2parse = messages_buffer->front();
				messages_buffer->pop();
				cout << getEpoch() << "::message2parse = " << message2parse << endl;
				msg2send_str* hdr = parseMessage(message2parse);
				/*cout << "msg_id " << hdr->msg_id << " des_id " << hdr->des_id << " tec_id " << hdr->tec_id << endl;*/
/*
				usleep (min_sleeping_time); //sleep(); if sec.*/
				//CHECK IF hdr->tec_id is in my phys
				if(hdr->des_id > 0 && hdr->tec_id > 0 && hdr->msg_id > 0){
					if(pmDriver.find(hdr->tec_id)!=pmDriver.end()) {
						cout << getEpoch() << "_UNICAST_Tx_dest:" << hdr->des_id << "_tec_id:" 
							<< hdr->tec_id << endl;
						transm_file_stats << getEpoch() << "_UNICAST_Tx_dest:" << hdr->des_id 
							<< "::tec_id:" << hdr->tec_id << endl;
						k_o ? transmitBurst(pmDriver[hdr->tec_id],hdr->des_id, hdr->data) :
						    	transmit(pmDriver[hdr->tec_id],hdr->des_id, hdr->data, ACK);
						phy_pending_msg[hdr->tec_id] = hdr->msg_id;
					}
				}
				else{ //BROADCAST MESSAGE via all tec--> NO ACK
					cout << getEpoch() << "_BROADCAST_dest:" << hdr->des_id << endl;
					transm_file_stats << getEpoch() << "_BROADCAST_dest:" << hdr->des_id << endl;
					for(DriversMap::iterator driver_iter = pmDriver.begin();
		        driver_iter != pmDriver.end();
		        driver_iter++) 
					{
						transmit(driver_iter->second,BROADCAST_ADD, hdr->data, NOACK);
					}
				}
			} 
		}
		else {
			//loop over all the PHY in the following way
			for(DriversMap::iterator driver_iter = pmDriver.begin();
        driver_iter != pmDriver.end();
        driver_iter++) {
				modemStatus_old = driver_iter->second->getStatus();
				modemStatus = driver_iter->second->updateStatus();
				//TODO: CHECK ACK STATUS 
				if(driver_iter->second->getAckStatus() != ACK_PENDING && 
					phy_pending_msg.find(driver_iter->first) != phy_pending_msg.end()) {
					bool ack_status = driver_iter->second->getAckStatus() == ACK_CONFIRMED;
					reportAck(phy_pending_msg[driver_iter->first], driver_iter->first, 
						ack_status, dir_label);
					cout << getEpoch() << "_ACK_confirmed:" << ack_status << "_from_PHY:" 
							 << driver_iter->first << "_for_packet:" 
							 << phy_pending_msg[driver_iter->first] << endl;
					transm_file_stats << getEpoch() << getEpoch() << "_ACK confirmed:" 
							 << ack_status << "_from_PHY:" << driver_iter->first << "_for_packet:" 
							 << phy_pending_msg[driver_iter->first] << endl;
					phy_pending_msg.erase(driver_iter->first);
				}

				//check if there is something received and write it in the log
				if (modemStatus == MODEM_IDLE_RX && modemStatus_old == MODEM_RX) {
					std::string rx_msg = driver_iter->second->getRxPayload();
					int src = driver_iter->second->getSrc();
					printToRxFile(src, driver_iter->first, rx_counter++, rx_msg, dir_label);
					cout << getEpoch() << "_Rx Message[" << ID << "]:" << rx_counter 
							 << "_From:" << src << "_via_PHY:"<< driver_iter->first << "_data:"
							 << rx_msg << endl;
					recv_file_stats << getEpoch() << "_Rx Message[" << ID << "]:" << rx_counter 
							 << "_From:" << src << "_via_PHY:"<< driver_iter->first << "_data:" 
							 << rx_msg << endl;
					driver_iter-> second -> resetModemStatus();
				}
			}
		}
		usleep(min_sleeping_time);
		/*usleep(1000000);*/

/*		transm_file_stats << "[" << getEpoch() << "]:: Send from " << ID << " to " << RECEIVER 
			       << " " << complete_message << "packet_counter = " << packet_counter << endl;
    // The last packet of the burst is sent in ack mode
		usleep(min_sleeping_time);
    // Retrieve the propagation delay thanks to the ACK
		int delay = pmDriver1->getDelay();
		cout << "Delay = " << delay << endl;
		delay_file_stats << "[" << getEpoch() << "]:: Delay = " << delay << endl;*/
	}
	return 0;
}

