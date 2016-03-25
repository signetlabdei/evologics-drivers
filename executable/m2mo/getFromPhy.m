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
% Receive a 8 byte packet  from the input buffer                          %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% INPUT:                                                                  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% OUTPUT:                                                                 %
% @RxPacket packet received from the input buffer                         %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function RxPacket = getFromPhy()

global outFolder

RxPacket = [];

DataFileName = strcat(outFolder, '/FromPhy2Mac');
FileName = strcat(outFolder, '/PhyRx*.txt');
FileExistNum = dir(FileName);
for ind = 1: length(FileExistNum)
    CurrentFileName = FileExistNum(ind).name;
    loc = strfind(CurrentFileName, 'Rx');
    x = CurrentFileName(loc+2:end);

    CurrentDataFileName = [DataFileName, x];
    delete(strcat(outFolder,'/',CurrentFileName)); 

    FileDataExistNum = dir(CurrentDataFileName);
    if ~isempty(FileDataExistNum)
        fid = fopen(CurrentDataFileName, 'r');
        S = fscanf(fid,'%s');
        fclose(fid);
        delete(CurrentDataFileName);

        Data = dec2bin(hex2dec(S), 32);

        Source = bin2dec(Data(1: 5));

        DestBits = Data(6:10);
        DestVec = find(DestBits == '1');

        MsgID = bin2dec(Data(11:20));

        TypeBits = Data(21:23);
        switch(TypeBits)
            case('000')
                Type = 'DATA';
        end

        TimeID = bin2dec(Data(24:end));

        CurrentRxPacket.Source = Source;
        CurrentRxPacket.DestVec = DestVec;
        CurrentRxPacket.MsgID = MsgID;
        CurrentRxPacket.Type = Type;
        CurrentRxPacket.TimeID = TimeID;

        RxPacket = [RxPacket, CurrentRxPacket];
        [status,curr_epoch] = system('date +%s%N | cut -b1-13');
        disp([curr_epoch(1:end-1), ' RxPacket']);
            
    end
end


return;
