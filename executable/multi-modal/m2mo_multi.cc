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
#include <map>

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
void printToFile(int src, int tec_id, std::string rx_msg, char * folder) { 
	char log_name [100];
	sprintf(log_name, "%s%s%d%s%d%s%s", folder, rx_log_header, tec_id, "_", src, rx_log_tail, extension_);
	/*std::cout << log_name << endl;*/
	std::ofstream out_file_stats;
	out_file_stats.open(log_name, std::ofstream::out); //erase and create a new file
	out_file_stats << src << "," << rx_msg << std::endl;
	out_file_stats.close();
	sprintf(log_name, "%s%s%d%s%d%s", folder, check_rx_log_, tec_id, "_", src, extension_);
	out_file_stats.open(log_name, std::ofstream::out); //erase and create a new file
	out_file_stats.close();
}

/**
 * Main: the transmitter
**/
 //TODO: decide how to set number and identifier of phys and their sockets
int main(int argc, char* argv[]) {
	// CHECK parameters
  cout << "Ciao" << endl;
	if (argc < 7) {
		cerr << "Usage:" <<endl;
    cerr << "./mycc ID ip1 port1 ip2 port2 keep-online" << endl;
		return -1;
	}
  // INITIALIZATION
  int ID = atoi(argv[1]);
	char* dir_label = ".";
	std::string ip1 = argv[2];
	std::string port1 = argv[3];
	std::string ip2 = argv[4];
	std::string port2 = argv[5];
	bool k_o = bool(atoi(argv[6]));
	bool set_id = true;
	
  // SET LOG FILES
	std::stringstream transm_file;
	std::stringstream delay_file;
	std::ofstream transm_file_stats(0);
	std::ofstream delay_file_stats(0);
        transm_file << "./sender_log.out";

  // CONNECT to the modem
  std::map<int,MdriverS2C_EvoLogics*> pmDriver;
  pmDriver[1] = connectModem(ip1, port1, ID, set_id, "tx_socket1.log");
  pmDriver[2] = connectModem(ip2, port2, ID, set_id, "tx_socket2.log");
  std::map<int,int> phy_pending_msg;
	usleep(400000);
	cout << "ID " << ID << " socket: " << ip1 <<":" << port1 << endl;
	cout << "ID " << ID << " socket: " << ip2 <<":" << port2 << endl;
	sleep(1);
  
  // RESET THE MODEM AFTER THE INITIALIZATION

  int modemStatus_old;
	int modemStatus;
	for(std::map<int,MdriverS2C_EvoLogics*>::iterator driver_iter = pmDriver.begin();
    driver_iter != pmDriver.end();
    driver_iter++) {
		modemStatus_old = driver_iter->second->getStatus();
		modemStatus = driver_iter->second->updateStatus();
		driver_iter-> second -> resetModemStatus();
	}
  // START THE APPLICATION
	//main cicle
	std::queue<std::string> *messages_buffer;
	std::string message2parse = "";
	while (true) {


		bool tx_flag = checkTxLog(dir_label);
		//cout << "flag = " << tx_flag << endl;
		if (tx_flag) {
			cout << "something to tx" << endl;
			//retreive sending time and data to send
			messages_buffer = readMessages(dir_label);
			while (!messages_buffer->empty()) {
				message2parse = messages_buffer->front();
				messages_buffer->pop();
				cout << getEpoch() << " message2parse = " << message2parse << endl;
				msg2send_str* hdr = parseMessage(message2parse);

				usleep (min_sleeping_time); //sleep(); if sec.
				//CHECK IF hdr->tec_id is in my phys
				if(pmDriver.find(hdr->tec_id)!=pmDriver.end()) {
					if(hdr->des_id > 0){
						k_o ? transmitBurst(pmDriver[hdr->tec_id],hdr->des_id, hdr->data) :
						    	transmit(pmDriver[hdr->tec_id],hdr->des_id, hdr->data, ACK);
						phy_pending_msg[hdr->tec_id] = hdr->msg_id;
					}
					else{ //BROADCAST MESSAGE --> NO ACK
						transmit(pmDriver[hdr->tec_id],BROADCAST_ADD, hdr->data, NOACK);
					}
				}
			} 


/*			//TODO: CHECK ACK STATUS 
			while(pmDriver[1]->getAckStatus() == ACK_PENDING) {
				if()
				usleep(500000);
				//cout <<"waiting ack";
				pmDriver[1]->updateStatus();
			}

			pmDriver[1]->getAckStatus() == ACK_CONFIRMED ? cout << "ACK confirmed!" << endl :  cout << "FAILED!" << endl;
			//cout << "Inviato " << ID << " to " << RECEIVER << " " << complete_message << endl;
			transm_file_stats << "[" << getEpoch() << "]:: Send from " << ID << endl;
	    // The last packet of the burst is sent in ack mode
			usleep(min_sleeping_time);    
			std::cout << getEpoch() << " Sent " << ID  << endl;*/
		}
		else {
			//loop over all the PHY in the following way
			for(std::map<int,MdriverS2C_EvoLogics*>::iterator driver_iter = pmDriver.begin();
        driver_iter != pmDriver.end();
        driver_iter++) {
				modemStatus_old = driver_iter->second->getStatus();
				modemStatus = driver_iter->second->updateStatus();
				//TODO: CHECK ACK STATUS 
				if(driver_iter->second->getAckStatus() != ACK_PENDING && 
					phy_pending_msg.find(driver_iter->first) != phy_pending_msg.end()) {
					driver_iter->second->getAckStatus() == ACK_CONFIRMED ? 
						cout << "ACK confirmed for packet!" << phy_pending_msg[driver_iter->first] << endl :
						cout << "FAILED!" << phy_pending_msg[driver_iter->first] << endl;
					phy_pending_msg.erase(driver_iter->first);
				}

				//check if there is something received and write it in the log
				if (modemStatus == MODEM_IDLE_RX && modemStatus_old == MODEM_RX) {
					std::string rx_msg = driver_iter->second->getRxPayload();
					int src = driver_iter->second->getSrc();
					printToFile(src,1,rx_msg,dir_label);
					cout << getEpoch() << " Rx Message " << ID << " From " << src << " " << rx_msg << endl;
					driver_iter-> second -> resetModemStatus();
				}
			}
		}
		usleep(min_sleeping_time);
		usleep(1000000);

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

