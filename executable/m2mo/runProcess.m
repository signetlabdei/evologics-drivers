% Copyright (c) 2016 Regents of the SIGNET lab, University of Padova.
% All rights reserved.
%
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions
% are met:
% 1. Redistributions of source code must retain the above copyright
%    notice, this list of conditions and the following disclaimer.
% 2. Redistributions in binary form must reproduce the above copyright
%    notice, this list of conditions and the following disclaimer in the
%    documentation and/or other materials provided with the distribution.
% 3. Neither the name of the University of Padova (SIGNET lab) nor the 
%    names of its contributors may be used to endorse or promote products 
%    derived from this software without specific prior written permission.
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
% "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
% TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
% PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
% CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
% EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
% PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
% OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
% WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
% OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
% ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

% @Authors: Filippo Campagnaro, Roee Diamant

% DESCRIPTION
% Authors: Filippo Campagnaro, Roee Diamant
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Generate and handle packet with constant size and period                %
% Returns the list of packet transmitted, received and delivery delay     %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% INPUT:                                                                  %
% @startexpTime: time when to start the generation [hour min sec]         %
% @currentNode Current Node ID                                            %
% @dest destination Node ID                                               %
% @expTime process duration, in seconds [s]                               %
% @period packet generation period                                        %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% OUTPUT:                                                                 %
% @txMsgVec vector with transmitted packets                               %
% @rxMsgVec vector with received packets                                  %
% @delayMsgVec vector with received packets' delivery delay               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [txMsgVec, rxMsgVec, delayMsgVec] = runProcess(startexpTime, ... 
                                        currentNode, dest, expTime, period)

RxFileName = ['RxFileNode_', num2str(currentNode)];
TxFileName = ['TxFileNode_', num2str(currentNode)];
countMsgID = (currentNode-1)*1000;
%Wait for begining of Experiement
StartTime = clock;
StartTime(4:6) = startexpTime;
disp(['I am Node: ', num2str(currentNode),'. Waiting till Exp Starts: ', ...
    num2str(etime(StartTime, clock)), ' [sec]']);
while (etime(clock, StartTime) < 0)
    pause(0.05);
end

delayMsgVec = [];
rxMsgVec = [];
txMsgVec = [];
sleeping_time = 0.0001;
sending_gap = 1;
sending_time = etime(clock, StartTime) + period;
while (etime(clock, StartTime) < expTime)
    timeDiff = etime(clock, StartTime);
    if (timeDiff >= sending_time)
        %Transmit
        countMsgID = countMsgID + 1;
        TxPacket = [];
        TxPacket.Source = currentNode;
        TxPacket.DestVec = dest;
        TxPacket.Type = 'DATA';
        TxPacket.MsgID = countMsgID;
        TxPacket.TimeID = timeDiff;
        TxPacket.Backoff = 0;
        
        [~,curr_epoch] = system('date +%s%N | cut -b1-13');
        disp([curr_epoch(1:end-1), ' Transmitting ', TxPacket.Type, ...
            ' for Msg: ', num2str(TxPacket.MsgID), ' to node(s) ', num2str(dest)]);
        TxTime = clock;
        %TxTime(6) = round(TxTime(6) + sending_gap);
        TxTime(6) = TxTime(6) + sending_gap;
        if TxTime(6) >= 60
            TxTime(6) = TxTime(6) - 60;
            TxTime(5) = TxTime(5) + 1;
            if TxTime(5) >= 60
                TxTime(5) = TxTime(5) - 60;
                TxTime(4) = TxTime(4) + 1;
            end
        end
        TxPacket.TxTime = TxTime(4:6);
        disp(['Transmitting'])
        %Transmit packet. Data: [TxPacket.Source, TxPacket.destVec, TxPacket.Type, TxPacket.MsgID]
        tx2Phy(TxPacket);
        save(TxFileName, 'TxPacket', 'timeDiff');
        txMsgVec = [txMsgVec, countMsgID];
        sending_time = sending_time + period;
    end
    %Check if Phy has new packet
    RxPacket = getFromPhy();
    if ~(isempty(RxPacket))
        %RxPacket found. [RxPacket.TxID, RxPacket.destVec, RxPacket.Type]
        save(RxFileName, 'RxPacket', 'timeDiff');
        rxMsgVec = [rxMsgVec, RxPacket.MsgID];
        delayMsgVec = [delayMsgVec, timeDiff-RxPacket.TimeID];
    end
    
end
    pause(sleeping_time);
    
end

