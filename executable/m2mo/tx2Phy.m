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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Send a 8 byte packet to the output buffer                               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% INPUT:                                                                  %
% @TxPacket: packet that has to be sent to the output buffer              %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% OUTPUT:                                                                 %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function tx2Phy(TxPacket)

global outFolder
%[status,curr_epoch] = system('date +%s%N | cut -b1-13');
%disp([curr_epoch(1:end-1), ' Transmitting ', TxPacket.Type, ' for Msg: ', num2str(TxPacket.MsgID), ' to node(s) ', num2str(TxPacket.DestVec),' at time : ', num2str(TxPacket.TxTime)]);

    
Source = dec2bin(TxPacket.Source, 5);

MsgID = dec2bin(TxPacket.MsgID, 10);

DestVec = TxPacket.DestVec;
DestBits = zeros(1, 5);
DestBits(DestVec) = 1;
DestBits = num2str(DestBits);
DestBits = DestBits(1: 3: end);

TimeID = dec2bin(TxPacket.TimeID, 9);

TxTime = TxPacket.TxTime;
TxTimeBits = [num2str(TxTime(1)), ',', num2str(TxTime(2)), ',', ...
    num2str(TxTime(3)), ','];

Type = TxPacket.Type;
switch(Type)
    case('DATA')
        TypeBits = '000';
end

TxVec = [Source, DestBits, MsgID, TypeBits, TimeID];
TxVec = dec2hex(bin2dec(TxVec),8);

TextToPhy = [TxTimeBits, TxVec];

FileName = strcat(outFolder,'/FromMac2Phy.txt');
fid = fopen(FileName, 'w');
fprintf(fid,'%s\n', TextToPhy);
fclose(fid);

FileName = '/MacTx.txt';
fid = fopen(strcat(outFolder,FileName), 'w');
fclose(fid);

return;
