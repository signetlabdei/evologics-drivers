//
// Copyright (c) 2015 Regents of the SIGNET lab, University of Padova.
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
 * @file uwenea_opt_driver.cc
 * @author Filippo Campagnaro
 * \version 1.0.0
 * \brief Implementation of the UWMdriver class.
 */

#include "uwenea_opt_driver.h"

Enea_opt_driver::Enea_opt_driver(std::string pToDevice_, int serial_baudrate_):
debug_(0),
pmConnector(this, pToDevice_, serial_baudrate_ ),
pmInterpreter(this),
period_(0.01),
id_(0),
log_level_(OPT_LOG_LEVEL_INFO),
log_file_("/dev/null")
{  
  status = OPT_MODEM_IDLE;
  payload_tx = "";
  payload_rx = "";
  
  if (debug_ > 1 )
  {
    debug_ = 1;
  }
}
		
Enea_opt_driver::~Enea_opt_driver(){
}

std::string Enea_opt_driver::getLogFile()
{
  return log_file_; 
}

void Enea_opt_driver::setLogFile(std::string f)
{
  log_file_ = f; 
}

opt_log_level_t Enea_opt_driver::getLogLevel()
{ 
  return log_level_; 
}

void Enea_opt_driver::setLogLevel(opt_log_level_t l)
{ 
  log_level_ = l; 
}

void Enea_opt_driver::resetModemStatus() {
  status = OPT_MODEM_IDLE;

}
		
void Enea_opt_driver::updateTx(std::string ptx) {
  payload_tx = ptx;
}
		
void Enea_opt_driver::updateRx(std::string prx) {
  payload_rx = prx;
}

void Enea_opt_driver::printOnLog(opt_log_level_t log_level,string module, string message) {
  opt_log_level_t actual_log_level = getLogLevel();
  if (actual_log_level >= log_level)
  {
    outLog.open((getLogFile()).c_str(), ios::app);
    outLog << left << "[" << getEpoch() << "]::" << module 
           << "()::" << message << endl;
    outLog.flush();
    outLog.close();
  }
}

int Enea_opt_driver::getID() {
  return id_; 
}

void Enea_opt_driver::setID(int id) {
  id_ = id; 
}

void Enea_opt_driver::modemTx() {
  assert (status == OPT_MODEM_IDLE);
  status = OPT_MODEM_TX;
  std::string tx_msg = pmInterpreter.build_send(payload_tx.length(), payload_tx);
  pmConnector.writeToModem (tx_msg);
  //printOnLog(OPT_LOG_LEVEL_INFO,"OPT_DRIVER_",tx_msg);
  /*checkTx(payload_tx.length());*/

  /*status = OPT_MODEM_IDLE;*/
  updateStatus();
}

opt_modem_state_t Enea_opt_driver::updateStatus() {
 /* if(debug_)
    cout << "Enea_opt_driver::updateStatus() " << status;*/
  switch(status) {
    case OPT_MODEM_TX:
      status = OPT_MODEM_IDLE;
      if(debug_)
        cout << ", OPT_MODEM_TX --> OPT_MODEM_IDLE" << endl;
      break;
    case OPT_MODEM_CFG:
      status = OPT_MODEM_IDLE;
      if(debug_)
        cout << ", OPT_MODEM_CFG --> OPT_MODEM_IDLE" << endl;
      break;
    case OPT_MODEM_RX:
      status = OPT_MODEM_IDLE;
      if(debug_)
        cout << ", OPT_MODEM_RX --> OPT_MODEM_IDLE" << endl;
      break;
    case OPT_MODEM_CFG_RX:
      status = OPT_MODEM_CFG;
      if(debug_)
        cout << ", OPT_MODEM_CFG_RX --> OPT_MODEM_CFG" << endl;
      break;
    default:
      while(!pmConnector.queueMsg.empty()) {
        std::string from_modem = pmConnector.readFromModem();
        istringstream tokenize(from_modem);
        std::string token = "";
        while (getline(tokenize, token, '\n'))
          checkRx(token);
      }
     /* break;*/
  }
  return status;
}

void Enea_opt_driver::stop() {
  pmConnector.closeConnection();
}

void Enea_opt_driver::start() {
  printOnLog(OPT_LOG_LEVEL_DEBUG,"ENEA_OPT_MODEM","MODEM_START");
  pmConnector.openConnection();
  status = OPT_MODEM_IDLE;
}

void Enea_opt_driver::modemSetBitrate(int brate) {
  assert (status == OPT_MODEM_IDLE || status == OPT_MODEM_RX);
  status = status == OPT_MODEM_IDLE ? OPT_MODEM_CFG : OPT_MODEM_CFG_RX;
  std::string tx_msg = pmInterpreter.build_set_rate(brate);
  pmConnector.writeToModem (tx_msg);
  //status = (status == OPT_MODEM_CFG_RX) ? OPT_MODEM_RX : OPT_MODEM_IDLE;
  updateStatus();
}

void Enea_opt_driver::modemSetThreshold(int threshold){
  assert (status == OPT_MODEM_IDLE || status == OPT_MODEM_RX);
  status = status == OPT_MODEM_IDLE ? OPT_MODEM_CFG : OPT_MODEM_CFG_RX ;
  std::string tx_msg = pmInterpreter.build_set_thres(threshold);
  pmConnector.writeToModem (tx_msg);
  //status = (status == OPT_MODEM_CFG_RX) ? OPT_MODEM_RX : OPT_MODEM_IDLE;
  updateStatus();
}

void Enea_opt_driver::modemSetLED(int led_flag){
  assert (status == OPT_MODEM_IDLE || status == OPT_MODEM_RX);
  status = (status == OPT_MODEM_IDLE ? OPT_MODEM_CFG : OPT_MODEM_CFG_RX);
  std::string tx_msg = pmInterpreter.build_set_LED(led_flag);
  pmConnector.writeToModem (tx_msg);  
  //status = (status == OPT_MODEM_CFG_RX) ? OPT_MODEM_RX : OPT_MODEM_IDLE;
  updateStatus();
}

void Enea_opt_driver::modemSetAnalog(int an_mode){
  status = (status == OPT_MODEM_IDLE ? OPT_MODEM_CFG : OPT_MODEM_CFG_RX);
  std::string tx_msg = pmInterpreter.build_set_analog(an_mode);
  pmConnector.writeToModem (tx_msg);  
  //status = (status == OPT_MODEM_CFG_RX) ? OPT_MODEM_RX : OPT_MODEM_IDLE;
  updateStatus();
}

/*void Enea_opt_driver::checkConf() {
  if (status != OPT_MODEM_CFG || status != OPT_MODEM_CFG_RX)
    return ;
  int count = 0;
  std::string temp = "";
  do {
    checkRx(temp);
    usleep(getPeriod());
    count ++;
    temp = pmConnector.readFromModem();
  } while ( temp.find("OK") == string::npos &&
            temp.find("ERROR_CONF") == string::npos 
            && count < 100);//TODO:log result
}

void Enea_opt_driver::checkTx() {
  if (status != OPT_MODEM_TX)
    return ;
  std::string temp = "";
  int count = 0;
  do {
    checkRx(temp);
    usleep(getPeriod());
    count ++;
    temp = pmConnector.readFromModem();
  } while (temp.find("INVIO: ") == string::npos 
            && count < 100); 
      //TODO:log result
  if (count < 100)
    count = 0;
  do {
    usleep(getPeriod());
    count ++;
  } while (pmConnector.readFromModem().find(Enea_opt_driver::int2str(size)) 
            == string::npos && count < 100);//TODO:log result
  if (count < 100)
    pmConnector.readFromModem();
}
*/
void Enea_opt_driver::checkRx(std::string rx_msg) {
  if (debug_)
    cout << " checkRx(" << rx_msg << ")" << endl;
  if (rx_msg.find("recv") != string::npos) {
    if (debug_)
      cout << " checkRx : receved! " << endl;
    pmInterpreter.parse_recv(rx_msg.substr(rx_msg.find("recv")));
    // log recv
    switch (status) {
      case OPT_MODEM_CFG:
        status = OPT_MODEM_CFG_RX;
        break;
      case OPT_MODEM_IDLE:
        status = OPT_MODEM_RX;
        break;
      default:
        return;
    }
    // PARSER TOTAL
    // PARSE FACE1
    // PARSE FACE2
    // PARSE FACE3
    // CHECK crc
    // SAVE IN PACKET
  }
}

double Enea_opt_driver::getPeriod()
{ 
  return period_; 
}

void Enea_opt_driver::setPeriod(double period)
{ 
  period_ = period; 
}
