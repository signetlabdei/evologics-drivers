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
 * @file minterpreter_op.h
 * @author Filippo Campagnaro
 * \version 1.0.0
 * \brief Header of the class that is in charge of building/parsing the necessary messages to make the Enea_opt_driver able to communicate with the modem according to the modem interface.
 */


#ifndef MINTERPRETEROP_H
#define MINTERPRETEROP_H

#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cmath>

using namespace std;

/* @author: Riccardo Masiero
 * @date:   03/08/2016 
 * @brief:  The header of the MinterpreterOp class, to build/parse the necessary messages to
 *          make the Enea_opt_driver able to communicate with the modem according to their commands. 
 
 */

/** Class used to build or parse control messages (this class derives UWMinterpreter); currently, this class implements methods 
 * to build/parse:
 *  - simple configuration messages (e.g., setting of the modem ID)
 *  - messages involved in the transmission of an instant message (see AT*SENSIM command)
 * NOTE: for a detailed description of the messages build and parse in this class, please refer to the Enea Optical modem
 */

// Forward declaration to avoid dependence from UWMdriver.h.
class Enea_opt_driver; 

class MinterpreterOp {
    // Additional info (further than payload, SRC, DST -> see Enea_opt_driver) that may be stored for each last received packet

public:

    /** 
     * Class constructor.
     * 
     * @param pmDriver_  pointer to the Enea_opt_driver object to link with this UWMinterpreter object.
     */
    MinterpreterOp(Enea_opt_driver*);

    /**
     * Class destructor.
     */
    ~MinterpreterOp();

    // METHODS to BUILD MESSAGES

    /** 
     * Method to build an command message to send messages. 
     * 
     * @param _length the length in bytes of the message to send (max is 64 bytes).
     * @param _data message to send in form of a binary data array (string of _length chars)
     * @return tx_string the requested message.
     */
    std::string build_send(int _length, std::string _payload);

    /** 
     * Method to build an command message to set the transmission rate. 
     *
     * @param _rate the transmission bitrate (115200:2000000).
     * @return command, the requested command message.
     */
    std::string build_set_rate(int _rate);

    /** 
     * Method to build an command message to set the reception threshold. 
     *
     * @param _thresh the noise threshold (0:5000).
     * @return command, the requested command message.
     */
    std::string build_set_thres(int _thresh);

     /** 
      * Method to build an command message to set the transmission LEDs. 
      *
      * @param _flag flag to set which LEDs to employ in the transission:
      *              0 - blue; 1 - blue + white; 2 - white
      * @return command, the requested command message.
      */
    std::string build_set_LED(int _flag);


    std::string build_set_analog(int _an_mode);

    /**
     * Method to parse a reception message message. 
     * NOTE: this method calls Enea_opt_driver::updateRx(int,int,std::string).
     * @param at_string the received string.
     */
    void parse_recv(std::string rx_string);

protected:
    Enea_opt_driver * pmDriver; /**< Link to the UWMdriver object which contains this UWMinterpreter object. */
    int debug_; /**< Flag to enable debug mode (i.e., printing of debug messages) if set to 1. */
    int getDebug();
    unsigned short crc16(const unsigned char* data_p, int length);

};
#endif	/* MINTERPRETEROP_H */
