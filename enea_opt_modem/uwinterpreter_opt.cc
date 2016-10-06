//
// Copyright (c) 2015 Regents of the SIGNET lab, University of Padova.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
// 3. Neither the name of the University of Padova (SIGNET lab) nor the 
//  names of its contributors may be used to endorse or promote products 
//  derived from this software without specific prior written permission.
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
 * @file minterpreter_opt.cc
 * @author Filippo Campagnaro
 * \version 1.0.0
 * \brief Header of the class that is in charge of building/parsing the necessary messages to make the Enea_opt_driver able to communicate with the modem according to the modem interface.
 */

#include "uwinterpreter_opt.h"
#include "uwenea_opt_driver.h"
#include "enea_consts.h"
/*#include <boost/lexical_cast.hpp>*/
 
#include <assert.h>   /* assert */
#include <cctype>


/*static void hexdump(std::string name, std::string str)
{
  int len = str.size();
  const char *data = str.c_str();

  std::cout << name << "[" << len << "]: " << std::hex;
  for (int i = 0; i < len; i++)
  {
    std::cout.fill('0');
    std::cout.width(2);
    std::cout << std::right << (int)data[i];

    if (std::isalnum(data[i]) || std::ispunct(data[i]))
      std::cout << "(" << data[i] << ")";
    std::cout << " ";
  }

  std::cout.width(0);
  std::cout << std::dec << std::endl;
}*/

static std::string hexdumplog(std::string str)
{
  int len = str.size();
  const char *data = str.c_str();
  
  std::string str_out = "";
   
  for (int i = 0; i < len; i++)
  {
    if (std::isalnum(data[i]) || std::ispunct(data[i]))
      str_out += data[i];
	  else {
  	  std::string str;
  	  std::stringstream sstr("");
  	  sstr << "[" << std::hex << (unsigned int)(unsigned char) data[i] << std::dec << "]";
  	  sstr >> str;	
  	  str_out += str;
  	}
  }

  return str_out;
}


/*static std::string hexdumpdata(std::string str)
{
  size_t len = str.size();
  const char *data = str.c_str();
    
  std::string str_out = "";

  for (size_t i = 0; i < len; i++)
  {
    std::stringstream sstr("");
    sstr.fill('0');
    sstr.width(2);
    sstr << std::hex << (unsigned int) (unsigned char) data[i] << std::dec;
    cout << " " << sstr.str() ;
    str_out += sstr.str();
  }
  return str_out;
}


static std::string dechexdumpdata(std::string strHex)
{
  size_t unBytes = strHex.size() / 2;
  std::string str_out(unBytes, 0);
  for (size_t i = 0; i < unBytes; ++i)
  {
    std::istringstream in(strHex.substr(2*i, 2));
    int byte = 0;
    in >> std::hex >> byte;
    str_out[i] = byte;
  }
  return str_out;
}*/


MinterpreterOp::MinterpreterOp(Enea_opt_driver* pmDriver_){
  // Member initialization
  pmDriver = pmDriver_;
}
		
MinterpreterOp::~MinterpreterOp(){
}
	
std::string MinterpreterOp::build_send(int _length, std::string _payload)
{
/*  string at_string;
  stringstream astr("");
  astr << "send," << _length << "," ;
  astr >> at_string;	
  at_string =  at_string + _payload + "\n";*/

  string at_string;
  std::string new_payload = /*hexdumpdata*/(_payload);
  int new_length = new_payload.size();
  stringstream astr("");
  astr << "send," << new_length << "," ;
  astr >> at_string;  
  at_string =  at_string + new_payload + "\n";
	
  if (getDebug())
  {
    cout << "OPT_MODEM_MESSAGE_" << hexdumplog(at_string) << endl;
  }
  std::stringstream sstr("");
  string strlog;
  sstr << "OPT_MODEM_MESSAGE_TO_SEND=" << hexdumplog(at_string) << endl;
  sstr >> strlog;
  pmDriver->printOnLog(OPT_LOG_LEVEL_INFO,"OPT_MODEM_MESSAGE_",strlog);
  // Return the created at_string
  return at_string;
}

std::string MinterpreterOp::build_set_rate(int _rate)
{
  assert((int)(_rate >= 115200 && _rate <= 2000000));
  string at_string;
  stringstream astr("");
  astr << "set_rate," << _rate  << "\n";
  astr >> at_string;  
  
  if (getDebug())
  {
    cout << "OPT_MODEM_MESSAGE_" << hexdumplog(at_string) << endl;
  }
  std::stringstream sstr("");
  string strlog;
  sstr << "OPT_MODEM_MESSAGE_TO_SEND=" << hexdumplog(at_string) << endl;
  sstr >> strlog;
  pmDriver->printOnLog(OPT_LOG_LEVEL_INFO,"OPT_MODEM_MESSAGE_",strlog);
  // Return the created at_string
  return at_string;
}

std::string MinterpreterOp::build_set_thres(int _thres)
{
  assert((int)(_thres > 0 && _thres < 5000));
  string at_string;
  stringstream astr("");
  astr << "set_tresh," << _thres << "\n";
  astr >> at_string;  
  
  if (getDebug())
  {
    cout << "OPT_MODEM_MESSAGE_" << hexdumplog(at_string) << endl;
  }
  std::stringstream sstr("");
  string strlog;
  sstr << "OPT_MODEM_MESSAGE_TO_SEND=" << hexdumplog(at_string) << endl;
  sstr >> strlog;
  pmDriver->printOnLog(OPT_LOG_LEVEL_INFO,"OPT_MODEM_MESSAGE_",strlog);
  // Return the created at_string
  return at_string;
}

std::string MinterpreterOp::build_set_LED(int _flag)
{
  assert((int)(_flag >= 0 && _flag <= 2));
  string at_string;
  stringstream astr("");
  astr << "set_LED," << _flag  << "\n";
  astr >> at_string;  
  
  if (getDebug())
  {
    cout << "OPT_MODEM_MESSAGE_" << hexdumplog(at_string) << endl;
  }
  std::stringstream sstr("");
  string strlog;
  sstr << "OPT_MODEM_MESSAGE_TO_SEND=" << hexdumplog(at_string) << endl;
  sstr >> strlog;
  pmDriver->printOnLog(OPT_LOG_LEVEL_INFO,"OPT_MODEM_MESSAGE_",strlog);
  // Return the created at_string
  return at_string;
}

std::string MinterpreterOp::build_set_analog(int _an_mode)
{
  string at_string;
  stringstream astr("");
  astr << "analog_mode," << "\n";
  switch ((EneaAnalogMode)(_an_mode)) {
    case ENEA_AN_DISABLE:
      astr << "disable";
      break;
    case ENEA_AN_MASTER:
      astr << "master";
      break;
    case ENEA_AN_SLAVE:
      astr << "slave";
      break;
  }
  astr << "\n";
  astr >> at_string;  
  
  if (getDebug())
  {
    cout << "OPT_MODEM_MESSAGE_" << hexdumplog(at_string) << endl;
  }
  std::stringstream sstr("");
  string strlog;
  sstr << "OPT_MODEM_MESSAGE_TO_SEND=" << hexdumplog(at_string) << endl;
  sstr >> strlog;
  pmDriver->printOnLog(OPT_LOG_LEVEL_INFO,"OPT_MODEM_MESSAGE_",strlog);
  // Return the created at_string
  return at_string;
}

unsigned short MinterpreterOp::crc16(const unsigned char* data_p, int length)
{
  unsigned char x;
  unsigned short crc = 0xFFFF;

  while (length--){
    /*cout << (unsigned short) (*data_p) <<endl;*/
    x = crc >> 8 ^ *data_p++;
    x ^= x>>4;
    crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
  }
  return crc;
}

int MinterpreterOp::getDebug() { 
  return pmDriver -> getDebug(); 
}

void MinterpreterOp::parse_recv(std::string rx_string) {

  std::string _prefix, _f, _rec, _rx_power, _payload, _trash;
  unsigned short crc16_rx, crc16_calc;
  bool crc_flag = false;
  double rx_power = 0;

  std::stringstream sstr("");
  string strlog;
  sstr << "PARSE_RECV::OPT_MODEM_MESSAGE_TO_PARSE=" << hexdumplog(rx_string) << endl;
  sstr >> strlog;
  pmDriver->printOnLog(OPT_LOG_LEVEL_INFO,"OPT_MODEM_MESSAGE_",strlog);

  _payload = "";
 
  //_payload = rx_string.substr(offset);
  
  istringstream iastr(rx_string);
  getline(iastr, _prefix, ',');
  if (getDebug())
    cout << pmDriver->getEpoch() << " MinterpreterOp::parse_recv rx_string = " << rx_string << endl;
/*  std::queue<int> rx_faces;*/
  int f_id = 0;
  while (f_id < 3) {
    while(_f.size() <= 3 && f_id <3) {
      getline(iastr, _f, ',');
      f_id ++;
    }
    getline(iastr, _rec, ',');
    if (_f.size() > 3 ) {
      while (atoi(_f.substr(3).c_str()) >  (_rec.size() -2)) {
        getline(iastr, _trash, ',');
        //cout <<  "needed: " << atoi(_f.substr(3).c_str()) << " trash = " << _trash << ", red: " << (_rec.size()-2) <<endl;
        _rec.append(",");
        _rec.append(_trash);
        //cout <<  "new reed: " << _rec.size()-2 <<endl;
        _trash = "";
      }
    }
    //IF NEW_FIRMWARE: when rx power will be implemented
    getline(iastr, _rx_power, ',');
    double new_rx_pwr =atof(_rx_power.c_str());
    
    if(getDebug())
      cout << pmDriver->getEpoch() << " MinterpreterOp::parse_recv: face = " << _f << " _rec = " << _rec
         << "_rec.size() = " << _rec.size() -2;
    if (_f.size() > 3 && atoi(_f.substr(3).c_str()) == _rec.size() -2) {
      if(getDebug())
       cout << ", msg_size = " << atoi(_f.substr(3).c_str()) <<  ", data = " <<  _rec.data() 
             << " size = " << _rec.size() << endl;
      crc16_calc = crc16((unsigned char *)(_rec.substr(0,_rec.size()-2).data()),_rec.size()-2);  
      /*unsigned char* sData_source_ = (unsigned char*)malloc(_rec.size());
      memcpy(sData_source_, _rec.data(), _rec.size());
      crc16_calc = crc16(sData_source_,_rec.size()-2);  */
      const unsigned char *crc = (unsigned char*)_rec.substr(_rec.size()-2,2).c_str();
      crc16_rx=((unsigned short) (crc[0]) << 8 &0xFF00 | (unsigned short) (crc[1]) &0x00FF);

      //crc16_rx = boost::lexical_cast<unsigned short>(_rec.substr(_rec.size()-2));
      if(getDebug())
        cout << pmDriver->getEpoch() << " MinterpreterOp::parse_recv: crc16_calc = " << crc16_calc 
             << " crc16_rx = " << crc16_rx << endl;
      if(crc16_calc == crc16_rx) {
        _payload = _rec.substr(0,_rec.size()-2);
        crc_flag = true;
        //IF NEW_FIRMWARE: when rx power will be implemented
        rx_power = (new_rx_pwr > rx_power) ? new_rx_pwr : rx_power;
      }
      else {
        crc_flag = false;
        //IF NEW_FIRMWARE: when rx power will be implemented
        rx_power = (new_rx_pwr > rx_power) ? new_rx_pwr : rx_power;
      }
    }
    _f = "";
  }  
  if(_payload.size()) {
    if(getDebug())
      cout << pmDriver->getEpoch() << " MinterpreterOp::parse_recv: _payload size = " << _payload.size() << endl;
    pmDriver -> updateRx(/*dechexdumpdata*/(_payload));
  }
  /*rx_integrity = atof(_integrity.c_str());*/
}
