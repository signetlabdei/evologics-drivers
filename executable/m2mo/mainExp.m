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
% Initialize the Matlab to Modem (M2Mo) framework                         %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% INPUT:                                                                  %
% @currentNode Current Node ID                                            %
% @dest destination Node ID                                               %
% @period packet generation period                                        %
% @hour: hour of the current day when to start the generation process     %
% @min: minutes of the set hour when to start the generation process      %
% @sec: seconds when to start the generation process                      %
% @expTime process duration, in seconds [s]                              %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% OUTPUT:                                                                 %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function mainExp(currentNode, dest, period, hour, min, sec, expTime)

global outFolder
%clean former files
outFolder = num2str(currentNode);
mkdir(outFolder);
FileName = strcat(outFolder,'/FromMac2Phy.txt');
if exist(FileName, 'file') == 2
    delete(FileName);
end
FileName = strcat(outFolder,'/MacTx.txt');
if exist(FileName, 'file') == 2
    delete(FileName);
end

DataFileName = strcat(outFolder,'/FromPhy2Mac');
FileName = strcat(outFolder,'/PhyRx*.txt');
FileExistNum = dir(FileName);
for ind = 1: length(FileExistNum)
    CurrentFileName = FileExistNum(ind).name;
    TempName = CurrentFileName;
     disp([CurrentFileName])
    delete(strcat(outFolder,'/',CurrentFileName))
    loc = strfind(TempName, '.');
    x = TempName(loc-1:end);
    CurrentDataFileName = [DataFileName, x];
    FileDataExistNum = dir(CurrentDataFileName);
    if ~isempty(FileDataExistNum)
        delete(strcat(CurrentDataFileName))
    end
end
% starting programm
StartexpTime = [hour, min, sec];
rng(currentNode);
[TxMsgVec, RxMsgVec, DelayMsgVec] = runProcess(StartexpTime, currentNode, dest, expTime, period);

disp(['TxMsgVec : ', num2str(TxMsgVec)]);
disp(['RxMsgVec : ', num2str(RxMsgVec)]);
disp(['DelayMsgVec : ', num2str(DelayMsgVec)]);
