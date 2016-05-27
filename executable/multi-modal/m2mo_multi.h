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
 * \brief Header of multimodal matlab to modem 
 *
 */

#include <map>

struct msg2send_str {
	int msg_id;
	int des_id;
	int tec_id;
	string data;
};

typedef std::map<int,MdriverS2C_EvoLogics*> DriversMap;

static char const* check_tx_log_ = "/MacTx.txt";
static char const* tx_log_ = "/FromMac2Phy.txt";

static char const* check_rx_log_ = "/PhyRx";
//static char const* rx_log_ = "'PhyRx*.txt";
static char const* rx_log_header = "/FromPhy";
static char const* rx_log_tail = "toMac";

static char const* ack_log_header = "/ack_msg_";
static char const* extension_ = ".txt";

static char const token_separator_ = ',';

const int min_sleeping_time = 100; // 5 200 ms 

bool checkTxLog(char * folder);

std::queue<std::string>* readMessages(char * folder);

msg2send_str* parseMessage(std::string message2parse);

void printToRxFile(int src, int tec_id, std::string rx_msg, char * folder);

void reportAck(int msg_id, int tec_id, bool confirmed, char * folder);