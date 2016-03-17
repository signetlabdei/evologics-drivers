function MainExp(CurrentNode, TopologyNum, Hour, Min, Sec)
%Main for experiment

global NodeNum


%channel param
Gamma = 2;
Alfa = 3;
m = 2;
SL = 154;
NL = 38;
BW = 3e3;
PeSymbolTh = 0.006;
c = 1500;
ChannelParam = [Gamma, Alfa, m, SL, NL, BW, PeSymbolTh,c];
MaxTpd = 3000 / c;

GenVar = 0.15;
MPR_Th = 0.9;
MessageLen = 1; %sec
SyncErr = 5; %sec
CheckTxBeforeSlot = 2;
ExpTime = 600;

%-----------------------------
%topology: change according to sea trial setting
NodeNum = 5;

Topology = ones(NodeNum,NodeNum);

switch(TopologyNum)
    case(1)
        NodeX = [0, 800, 900, 700, 10];
    case(2)
        NodeX = [0, 800, 900, 10, 10];
    case(3)
        NodeX = [0, 800, 900, 100, 10];
    case(4)
        NodeX = [0, 50, 50, 900, 10];
end
NodeX = NodeX - min(NodeX);
NodeY = zeros(1,NodeNum);



DistMat = zeros(NodeNum, NodeNum);
for n=1:NodeNum
    for k=1:NodeNum
        if n~=k
            DistMat(n,k) = sqrt((NodeX(n)-NodeX(k))^2 + (NodeY(n)-NodeY(k))^2);
        end
    end
end
Tpd_info = DistMat ./ c;

%clean former files
FileName = 'FromMac2Phy.txt';
if exist(FileName, 'file') == 2
    delete(FileName);
end
FileName = 'MacTx.txt';
if exist(FileName, 'file') == 2
    delete(FileName);
end

DataFileName = 'FromPhy2Mac';
FileName = 'PhyRx*.txt';
FileExistNum = dir(FileName);
for ind = 1: length(FileExistNum)
    CurrentFileName = FileExistNum(ind).name;
    TempName = CurrentFileName;
    delete(CurrentFileName)
    
    loc = strfind(TempName, '.');
    x = TempName(loc-1:end);
    CurrentDataFileName = [DataFileName, x];
    FileDataExistNum = dir(CurrentDataFileName);
    if length(FileDataExistNum) > 0
        delete(CurrentDataFileName)
    end
end

% while(1)
%     CurrentTime = clock;
%     prompt={'Enter Node Number:', 'Enter Start time [hour]:', 'Enter Start time [min]:', 'Enter Start time [sec]:'};
%     name=['Input for Sea Experiement. Current Time: ', num2str(round(CurrentTime(4:6)))];
%     numlines=[1 100];
%     defaultanswer={'',num2str(CurrentTime(4)), num2str(CurrentTime(5)+1), '0'};
%     answer=inputdlg(prompt,name, numlines,defaultanswer);
%     
%     CurrentNode = str2num(answer{1});
%     Hour = str2num(answer{2});
%     Min = str2num(answer{3});
%     Sec = str2num(answer{4});
%     StartExpTime = [Hour, Min, Sec];
%     if ~(isempty(CurrentNode)) && (length(StartExpTime) == 3)
%         break;
%     end
% end

StartExpTime = [Hour, Min, Sec];
rng(CurrentNode);
[TxMsgVec, RxMsgVec, DelayMsgVec] = NF_Exp(TopologyNum, StartExpTime, CurrentNode, Tpd_info, Topology, GenVar, ExpTime, SyncErr, CheckTxBeforeSlot, MessageLen, MaxTpd, MPR_Th, ChannelParam);

disp(['TxMsgVec : ', num2str(TxMsgVec)]);
disp(['RxMsgVec : ', num2str(RxMsgVec)]);
disp(['DelayMsgVec : ', num2str(DelayMsgVec)]);
