function [TxMsgVec, RxMsgVec, DelayMsgVec] = NF_Exp(TopologyNum, StartExpTime, CurrentNode, Tpd_info, Topology, GenVar, ExpTime, SyncErr, CheckTxBeforeSlot, MessageLen, MaxTpd, MPR_Th, ChannelParam)
%function [UpdateTxMsgRequestMat, MsgDelayRatioSum, RxMsgVec, RxMsgVecFull, PacketSliceCount] = NF_Exp(CurrentNode, Tpd_info, Topology, ConflictMat, TxMessageRequestMat, TxMessageDestMat, TxMsgIdMat, LastMsgID, ClockRes, MaxLoopID, MessageLen, MaxPacket, MaxTpd, MPRFlag, MPR_Th, ChannelParam)
%
%Description:
% This is the main function for slotted FAMA-near far simulation
%
%Input:
% CurrentNode - ID of current node
% Tpd_info - matrix of Tpd information with elements (x,y) being the Tpd between nodes x and y (if x and y are not one-hop neighbors, Tpd(x,y)=inf). It is set such that Kj [set of all nodes connected to j] is simply find(Tpd_info(j, :)>0)
% Topology - symmetric binary matrix of topology [NodeNum X NodeNum]
% ConflictMat - symmetric binary matrix of interference [NodeNum X NodeNum]
% TxMessageRequestMat - a binery matrix of requests for message transmission
% TxMessageDestMat - a matrix of destination nodes for messages
% TxMsgIdMat - matrix of message ID
% LastMsgID - highest message ID
% ClockRes - resolution of clock [sec]
% MaxLoopID - number of clock ticks for each simulation
% MessageLen - message duration [sec]
% MaxTpd - the maximum possible Tpd
% MPRFlag - a flag. 1 = ideal MPR, 2 = realistic MPR
% MPR_Th - threshold on the probability to succesfully receive MPR packets
% ChannelParam - [Gamma, Alfa, m, SL, NL, BW, Pd, c];
%
%Output:
% UpdateTxMsgRequestMat - a matrix of updated message requests (msg request while current node is occupied with own CS are ignored)
% MsgDelayRatioSum - average delay time for msg from request till succesfull finish [sec]
% RxMsgVec - a vector of arriving msg ID (for fairness measure)
% RxMsgVecFull - a vector of succesfull msg ID
% PacketSliceCount - a matrix of succesfull arriving packets

RxFileName = ['RxFileNode_', num2str(CurrentNode)];
TxFileName = ['TxFileNode_', num2str(CurrentNode)];

RTSQueue = {};
IncomingPacket = {};
OutgoingPacket = {};
temp1 = {};
temp1.packet = [];
State = {};
SilenceSource = {};

NodeNum = size(Tpd_info, 1);

RTSQueue{1} = temp1;
IncomingPacket{1} = temp1;
OutgoingPacket{1} = {};
State{1} = 'IDLE';
SilenceSource{1} = [];

SlotRes = MessageLen+MaxTpd+SyncErr+CheckTxBeforeSlot; %time slot resolution
MaxSlotID = floor(ExpTime/SlotRes); %number of time slots

CTSWait = -1*ones(1,1);
CTSWaitSource = -1*ones(1,1);
xRTSWait = -1*ones(1,1);
xDataWait = -1*ones(1,1);
DataWait = -1*ones(1,1);
BackoffTimer = -1*ones(1,1);
FreezeBackoff = zeros(1,1);

%Min and max backoff slots, backoff time is uniformly distributed between
%the min and max
MinBackoff = 2;
MaxBackoff = 5;

%Max backoff times
MaxBackoffTimes = 3;

%get matrix M
%----------------
M_1 = CalcMHandshake(Tpd_info.*ChannelParam(end), CurrentNode, ChannelParam);
% tM = M_1;
% for NodeInd1 = 1: NodeNum
%     for NodeInd2 = 1: NodeNum
%         if (M_1(NodeInd1, NodeInd2) == 0) && (M_1(NodeInd2, NodeInd1) == 1)
%             tM(NodeInd1, NodeInd2) = 1;
%         end
%     end
% end
% M = tM;
M = M_1;
P_MPRMat = M;
%----------------

SecondTxMessageRequestVec = poissrnd(GenVar, 1, MaxSlotID/2+1);

TxMessageRequestVec = zeros(1, MaxSlotID+1);
DestRequestVec = zeros(1, MaxSlotID+1);
switch(TopologyNum)
    case(1)
        switch(CurrentNode)
            case(1)
                CurrentRxReq = [4,17];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [5,4];
                SecondCurrentRxReq = [30+find(SecondTxMessageRequestVec > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(2)
                CurrentRxReq = [6, 15, 20];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [3, 3, 4];
                SecondCurrentRxReq = [30+find(SecondTxMessageRequestVec > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(3)
                CurrentRxReq = [2, 26];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [2, 5];
                SecondCurrentRxReq = [30+find(SecondTxMessageRequestVec > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(4)
                CurrentRxReq = [8, 10];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [1, 2];
                SecondCurrentRxReq = [30+find(SecondTxMessageRequestVec > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(5)
                CurrentRxReq = [12, 16, 22];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [1, 4, 1];
                SecondCurrentRxReq = [30+find(SecondTxMessageRequestVec > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
        end
    case(2)
        switch(CurrentNode)
            case(1)
                CurrentRxReq = [16];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [2];
                SecondCurrentRxReq = [35+find(SecondTxMessageRequestVec(6:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(2)
                CurrentRxReq = [4,26];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [3, 3];
                SecondCurrentRxReq = [35+find(SecondTxMessageRequestVec(6:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(3)
                CurrentRxReq = [10, 32];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [2, 2];
                SecondCurrentRxReq = [35+find(SecondTxMessageRequestVec(6:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(4)
                CurrentRxReq = [2, 20, 30];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [1, 3, 5];
                SecondCurrentRxReq = [35+find(SecondTxMessageRequestVec(6:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(5)
                CurrentRxReq = [8, 24];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [1, 4];
                SecondCurrentRxReq = [35+find(SecondTxMessageRequestVec(6:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
        end
    case(3)
        switch(CurrentNode)
            case(1)
                CurrentRxReq = [16];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [3];
                SecondCurrentRxReq = [33+find(SecondTxMessageRequestVec(4:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(2)
                CurrentRxReq = [4, 28];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [3, 3];
                SecondCurrentRxReq = [33+find(SecondTxMessageRequestVec(4:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(3)
                CurrentRxReq = [10, 22];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [4, 2];
                SecondCurrentRxReq = [33+find(SecondTxMessageRequestVec(4:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(4)
                CurrentRxReq = [2, 26];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [1, 5];
                SecondCurrentRxReq = [33+find(SecondTxMessageRequestVec(4:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(5)
                CurrentRxReq = [8, 20];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [1, 4];
                SecondCurrentRxReq = [33+find(SecondTxMessageRequestVec(4:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
        end
    case(4)
        switch(CurrentNode)
            case(1)
                CurrentRxReq = [18, 28];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [4, 4];
                SecondCurrentRxReq = [33+find(SecondTxMessageRequestVec(4:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(2)
                CurrentRxReq = [2, 26];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [1, 5];
                SecondCurrentRxReq = [33+find(SecondTxMessageRequestVec(4:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(3)
                CurrentRxReq = [10, 16];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [4, 2];
                SecondCurrentRxReq = [33+find(SecondTxMessageRequestVec(4:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(4)
                CurrentRxReq = [4, 22];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [2, 1];
                SecondCurrentRxReq = [33+find(SecondTxMessageRequestVec(4:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
            case(5)
                CurrentRxReq = [8, 20];
                TxMessageRequestVec(CurrentRxReq) = 1;
                DestRequestVec(CurrentRxReq) = [1, 2];
                SecondCurrentRxReq = [33+find(SecondTxMessageRequestVec(4:end) > 0)];
                TxMessageRequestVec(SecondCurrentRxReq) = 1;
        end
end

CountMsgID = (CurrentNode-1)*100;

%Wait for begining of Experiement
StartTime = clock;
StartTime(4:6) = StartExpTime;
disp(['I am Node: ', num2str(CurrentNode),'. Waiting till Exp Starts: ', num2str(etime(StartTime, clock)), ' [sec]']);
while (etime(clock, StartTime) < 0)
    pause(0.05);
end

DelayMsgVec = [];
RxMsgVec = [];
TxMsgVec = [];
TxSlotNum = [];
LastSlotID = 0;
TxNodeCTSDest = 0;
while (etime(clock, StartTime) < ExpTime)
    TimeDiff = etime(clock, StartTime);
    SlotID = floor(TimeDiff / SlotRes) + 1;
    
    if LastSlotID ~= SlotID
        disp(['Current Timeslot: ', num2str(SlotID), '. Current Node: ', num2str(CurrentNode)]);
        LastSlotID = SlotID;
    end
    
    %Timeout countdown
    if CTSWait>0
        CTSWait = CTSWait-1;
    end;
    if xRTSWait>0
        xRTSWait = xRTSWait-1;
    end;
    if xDataWait>0
        xDataWait = xDataWait-1;
    end;
    if DataWait>0
        DataWait = DataWait-1;
    end;
    if (BackoffTimer>0)&&~(FreezeBackoff)
        BackoffTimer = BackoffTimer-1;
    end;
    
    %Send the packet at the begining of each time slot
    RxPacketVec = [];
    %Loop till end of time slot
    NextSlotID = SlotID;
    while(SlotID == NextSlotID)
        TimeDiff = etime(clock, StartTime) + CheckTxBeforeSlot;
        NextSlotID = floor(TimeDiff / SlotRes) + 1;
        %Check if Phy has new packet
        RxPacket = GetFromPhy(CurrentNode);
        if ~(isempty(RxPacket))
            %RxPacket found. [RxPacket.TxID, RxPacket.DestVec, RxPacket.Type]
            RxPacketVec = [RxPacketVec, RxPacket];
        end
        pause(0.05);
    end
    
    if ~(isempty(RxPacketVec))
        %Process the incoming packets
        CurrentState = State{1};
        
        for ind = 1: length(RxPacketVec);
            if iscell(RxPacketVec)
                temp1 = RxPacketVec{ind};
                if iscell(temp1)
                    CurrentPacket = temp1{1};
                else
                    CurrentPacket = temp1;
                end
            else
                CurrentPacket = RxPacketVec(ind);
            end;
            
            switch(CurrentPacket.Type)
                case('RTS')
                    if (CurrentPacket.DestVec==CurrentNode)
                        %!!!!calculate possible extra nodes to transmit
                        CurrentSource = CurrentPacket.Source;
                        PossbileNodeTx = CalcPossibleNodes(CurrentSource, M(CurrentSource, :), P_MPRMat(CurrentSource, :), MPR_Th);
                        
                        switch(CurrentState)
                            case('IDLE')
                                Packet = [];
                                Packet.Source = CurrentNode;
                                Packet.DestVec = PossbileNodeTx;%!!!!CurrentPacket.Source;
                                Packet.Type = 'CTS';
                                Packet.MsgID = CurrentPacket.MsgID;
                                Packet.SlotID = CurrentPacket.SlotID;
                                Packet.Backoff = 0;
                                OutgoingPacket{1} = Packet; %send CTS
                                
                                LastState = State{1};
                                State{1} = 'WFDATA';
                                disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                            case('WFCTS')
                            case('SILENCE')
                            case('BACKOFF')
                                FreezeBackoff(1) = 1;
                                Packet = [];
                                Packet.Source = CurrentNode;
                                Packet.DestVec = PossbileNodeTx;%CurrentPacket.Source;
                                Packet.Type = 'CTS';
                                Packet.MsgID = CurrentPacket.MsgID;
                                Packet.SlotID = CurrentPacket.SlotID;
                                Packet.Backoff = 0;
                                OutgoingPacket{1} = Packet;
                                
                                LastState = State{1};
                                State{1} = 'WFDATA';
                                disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                        end;
                    else
                        LastState = State{1};
                        switch(CurrentState)
                            case('IDLE')
                                xRTSWait(1) = 2; %Wait two slots to receive xData
                                State{1} = 'SILENCE_xRTS';
                            case('WFCTS')
                                xRTSWait(1) = 2; %Wait two slots to receive
                                State{1} = 'SILENCE_xRTS';
                            case('SILENCE')
                            case('BACKOFF')
                                FreezeBackoff(1) = 1;
                                xRTSWait(1) = 2; %Wait two slots to receive xData
                                State{1} = 'SILENCE_xRTS';
                        end;
                        disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                    end;
                case('CTS')
                    %!!!!
                    if (sum(CurrentPacket.DestVec==CurrentNode))
                        switch(CurrentState)
                            case('IDLE')
                                %Modems in this exp doesn't have MPR, so do nothing
                                
                            case{'WFCTS', 'BACKOFF'}
                                %check if waiting for this CTS
                                if CTSWaitSource(1) == CurrentPacket.Source
                                    %Start data transmission
                                    Packet = [];
                                    Packet.Source = CurrentNode;
                                    Packet.DestVec = CurrentPacket.Source;
                                    Packet.Type = 'DATA';
                                    Packet.MsgID = CurrentPacket.MsgID;
                                    Packet.SlotID = CurrentPacket.SlotID;
                                    Packet.Backoff = 0;
                                    OutgoingPacket{1} = Packet;
                                    
                                    LastState = State{1};
                                    State{1} = 'TXDATA';
                                    disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                                    
                                    %Remove the RTS from the queue
                                    CurrentRTSQueue = RTSQueue{1}.packet;
                                    if length(CurrentRTSQueue)==1
                                        CurrentRTSQueue = [];
                                    else
                                        for ind2 = 1:length(CurrentRTSQueue)-1
                                            CurrentRTSQueue{ind2} = CurrentRTSQueue{ind2+1};
                                        end;
                                        CurrentRTSQueue(length(CurrentRTSQueue)) = [];
                                    end;
                                    RTSQueue{1}.packet = CurrentRTSQueue;
                                    
                                    %Reset CTSWait
                                    CTSWait(1) = -1;
                                    CTSWaitSource(1) = -1;
                                end
                                
                            case('SILENCE_xRTS')
                                %!!!!check if waiting for this CTS
                                if CTSWaitSource(1) == CurrentPacket.Source
                                    %Start data transmission
                                    Packet = [];
                                    Packet.Source = CurrentNode;
                                    Packet.DestVec = CurrentPacket.Source;
                                    Packet.Type = 'DATA';
                                    Packet.MsgID = CurrentPacket.MsgID;
                                    Packet.SlotID = CurrentPacket.SlotID;
                                    Packet.Backoff = 0;
                                    OutgoingPacket{1} = Packet;
                                    
                                    LastState = State{1};
                                    State{1} = 'TXDATA';
                                    disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                                    
                                    %Remove the RTS from the queue
                                    CurrentRTSQueue = RTSQueue{1}.packet;
                                    if length(CurrentRTSQueue) > 0
                                        if length(CurrentRTSQueue)==1
                                            CurrentRTSQueue = [];
                                        else
                                            for ind2 = 1:length(CurrentRTSQueue)-1
                                                CurrentRTSQueue{ind2} = CurrentRTSQueue{ind2+1};
                                            end;
                                            CurrentRTSQueue(length(CurrentRTSQueue)) = [];
                                        end;
                                        RTSQueue{1}.packet = CurrentRTSQueue;
                                    end
                                    
                                    %Reset CTSWait
                                    CTSWait(1) = -1;
                                    CTSWaitSource(1) = -1;
                                else
                                    xRTSWait(1) = -1;
                                    LastState = State{1};
                                    State{1} = 'IDLE';
                                    disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                                    %do not transmit to the true destination of the CTS
                                    TxNodePossible = CurrentPacket.DestVec;
                                    TxNodeCTSDest = TxNodePossible(1);
                                end
                        end;
                    else
                        if findstr(State{1},'BACKOFF')
                            FreezeBackoff(1) = 1;
                        end;
                        CurrentSilenceSource = SilenceSource{1};
                        if isempty(find(CurrentSilenceSource==CurrentPacket.Source, 1))
                            if isempty(CurrentSilenceSource)
                                CurrentSilenceSource= CurrentPacket.Source;
                            else
                                CurrentSilenceSource = [CurrentSilenceSource,CurrentPacket.Source];
                            end;
                        end;
                        SilenceSource{1} = CurrentSilenceSource;
                        LastState = State{1};
                        State{1} = 'SILENCE';
                        disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                        xRTSWait(1) = -1;
                        xDataWait(1) = 2;
                    end;
                    
                case('DATA')
                    if (CurrentPacket.DestVec==CurrentNode)
                        DataWait(1) = -1;
                        
                        MessageID = CurrentPacket.MsgID;
                        MsgSlotID = CurrentPacket.SlotID;
                        
                        RxMsgVec = [RxMsgVec, MessageID]; %#ok<AGROW>
                        DelayMsgVec = [DelayMsgVec, SlotID-MsgSlotID];
                        save(RxFileName, 'RxMsgVec', 'DelayMsgVec');
                        disp(['Received Msg ID: ', num2str(MessageID), ' with Delay: ', num2str(SlotID-MsgSlotID), ' time slots']);
                        
                        LastState = State{1};
                        State{1} = 'IDLE';
                        disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                        
                    else
                        xRTSWait(1) = -1;
                        if ~strcmp(State{1},'TXDATA')
                            xDataWait(1) = 0; %no ACK in exp
                        end;
                        
                        CurrentSilenceSource = SilenceSource{1};
                        ind2 = find(CurrentSilenceSource==CurrentPacket.Source);
                        CurrentSilenceSource(ind2) = [];
                        SilenceSource{1} = CurrentSilenceSource;
                        if isempty(CurrentSilenceSource)
                            if ~isempty(RTSQueue{1}.packet)
                                LastState = State{1};
                                State{1} = 'BACKOFF';
                                disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                                if FreezeBackoff(1) ==1
                                    FreezeBackoff(1) = 0;
                                else
                                    BackoffTimer(1) = MinBackoff+round(rand()*(MaxBackoff-MinBackoff));
                                    FreezeBackoff(1) = 0;
                                    disp(['Backoff for: ', num2str(BackoffTimer(1)), ' time slots']);
                                end
                            else
                                LastState = State{1};
                                State{1} = 'IDLE';
                                disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                            end;
                        end;
                    end;
            end;
        end
    end
    IncomingPacket{1}.packet = [];
    
    %Deal with timeout
    
    %CTS timeout goes to backoff
    if CTSWait(1)==0
        if (xRTSWait(1)>0)
            LastState = State{1};
            State{1} = 'SILENCE_xRTS';
            disp(['Going from State: ', LastState, ', into state: ', State{1}]);
        elseif ~strcmp(State{1},'SILENCE')
            LastState = State{1};
            State{1} = 'BACKOFF';
            disp(['Going from State: ', LastState, ', into state: ', State{1}]);
            if FreezeBackoff(1) == 1
                FreezeBackoff(1) = 0;
            else
                BackoffTimer(1) = MinBackoff+round(rand()*(MaxBackoff-MinBackoff));
                FreezeBackoff(1) = 0;
                disp(['Backoff for: ', num2str(BackoffTimer(1)), ' time slots']);
            end;
            CTSWait(1) = -1;
        end;
    end;
    
    %xRTS timeout goes to idle
    if xRTSWait(1)==0
        if (~strcmp(State{1},'TXDATA')&&~strcmp(State{1},'SIELENCE'))
            if FreezeBackoff(1) == 1
                FreezeBackoff(1) = 0;
                LastState = State{1};
                State{1} = 'BACKOFF';
                disp(['Going from State: ', LastState, ', into state: ', State{1}]);
            else
                if ~isempty(RTSQueue{1}.packet)
                    LastState = State{1};
                    State{1} = 'BACKOFF';
                    disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                    if FreezeBackoff(1) == 1
                        FreezeBackoff(1) = 0;
                    else
                        BackoffTimer(1) = MinBackoff+round(rand()*(MaxBackoff-MinBackoff));
                        FreezeBackoff(1) = 0;
                        disp(['Backoff for: ', num2str(BackoffTimer(1)), ' time slots']);
                    end;
                else
                    LastState = State{1};
                    State{1} = 'IDLE';
                    disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                end;
            end;
        end;
        xRTSWait(1) = -1;
    end;
    
    %xData timeout goes to backoff or idle
    if xDataWait(1)==0
        if ~isempty(RTSQueue{1}.packet)
            LastState = State{1};
            State{1} = 'BACKOFF';
            disp(['Going from State: ', LastState, ', into state: ', State{1}]);
            if FreezeBackoff(1) == 1
                FreezeBackoff(1) = 0;
            else
                BackoffTimer(1) = MinBackoff+round(rand()*(MaxBackoff-MinBackoff));
                FreezeBackoff(1) = 0;
                disp(['Backoff for: ', num2str(BackoffTimer(1)), ' time slots']);
            end;
        else
            LastState = State{1};
            State{1} = 'IDLE';
            disp(['Going from State: ', LastState, ', into state: ', State{1}]);
        end;
        xDataWait(1) = -1;
    end;
    
    %Data timeout goes to backoff or idle
    if DataWait(1)==0
        if FreezeBackoff(1) == 1
            FreezeBackoff(1) = 0;
            LastState = State{1};
            State{1} = 'BACKOFF';
            disp(['Going from State: ', LastState, ', into state: ', State{1}]);
        else
            if ~isempty(RTSQueue{1}.packet)
                LastState = State{1};
                State{1} = 'BACKOFF';
                disp(['Going from State: ', LastState, ', into state: ', State{1}]);
                if FreezeBackoff(1) == 1
                    FreezeBackoff(1) = 0;
                else
                    BackoffTimer(1) = MinBackoff+round(rand()*(MaxBackoff-MinBackoff));
                    FreezeBackoff(1) = 0;
                    disp(['Backoff for: ', num2str(BackoffTimer(1)), ' time slots']);
                end;
            else
                LastState = State{1};
                State{1} = 'IDLE';
                disp(['Going from State: ', LastState, ', into state: ', State{1}]);
            end;
        end;
    end;
    
    %Backoff timout goes to idle
    if (BackoffTimer(1)==0)&&~(FreezeBackoff(1))
        CurrentRTSQueue = RTSQueue{1}.packet;
        if length(CurrentRTSQueue) > 0
            if iscell(CurrentRTSQueue)
                CurrentRTS = CurrentRTSQueue{1};
            else
                CurrentRTS = CurrentRTSQueue(1);
            end;
            if iscell(CurrentRTS)
                CurrentRTS = CurrentRTS{1};
            end;
            CurrentRTS.Backoff = CurrentRTS.Backoff+1; % add backoff times
            
            if (CurrentRTS.Backoff==MaxBackoffTimes)
                %Remove the RTS from the queue
                if length(CurrentRTSQueue)==1
                    CurrentRTSQueue = [];
                else
                    for ind = 1:length(CurrentRTSQueue)-1
                        CurrentRTSQueue{ind} = CurrentRTSQueue{ind+1};
                    end;
                    CurrentRTSQueue(length(CurrentRTSQueue)) = [];
                end;
            else
                if iscell(CurrentRTSQueue)
                    CurrentRTSQueue{1} = CurrentRTS;
                else
                    CurrentRTSQueue(1) = CurrentRTS;
                end;
            end;
            
            RTSQueue{1}.packet = CurrentRTSQueue;
        end
        
        LastState = State{1};
        State{1} = 'IDLE';
        disp(['Going from State: ', LastState, ', into state: ', State{1}]);
        BackoffTimer(1) = -1;
    end;
    
    if 1 %strcmp(State{1},'IDLE')||strcmp(State{1},'BACKOFF')
        TimeDiff = etime(clock, StartTime);
        NextSlotID = floor(TimeDiff / SlotRes) + 2;
        %Check for new message
        if TxMessageRequestVec(NextSlotID) > 0
            Vec = Topology(CurrentNode, :);
            Vec(CurrentNode) = 0;
            if TxNodeCTSDest > 0
                Vec(TxNodeCTSDest) = 0;
                TxNodeCTSDest = 0;
            end
            Dest = DestRequestVec(NextSlotID);
            if Dest == 0
                PosDest = find(Vec);
                Dest = PosDest(randi(length(PosDest)));
            end
            
            %Genereate the RTS
            CountMsgID = CountMsgID + 1;
            
            Packet = [];
            Packet.Source = CurrentNode;
            Packet.DestVec = Dest;
            Packet.Type = 'RTS';
            Packet.MsgID = CountMsgID;
            Packet.SlotID = NextSlotID;
            Packet.Backoff = 0;
            
            disp(['Transmitting RTS for Msg ID: ', num2str(CountMsgID),' at time slot: ', num2str(NextSlotID)]);
            TxMsgVec = [TxMsgVec, CountMsgID];
            TxSlotNum = [TxSlotNum, NextSlotID];
            save(TxFileName, 'TxMsgVec', 'TxSlotNum');
            
            %Insert the RTS into the queue
            temp = RTSQueue{1};
            CurrentRTSQueue = temp.packet;
            if length(CurrentRTSQueue)==1
                CurrentRTSQueue = {CurrentRTSQueue, Packet}; %#ok<AGROW>
            elseif isempty(CurrentRTSQueue)
                CurrentRTSQueue = Packet;
            else
                CurrentRTSQueue = [CurrentRTSQueue, Packet];
            end;
            temp.packet = CurrentRTSQueue;
            RTSQueue{1} = temp;
            
        end
    end;
    
    CurrentRTSQueue = RTSQueue{1}.packet;
    if ~isempty(CurrentRTSQueue)
        CurrentState = State{1};
        if findstr(CurrentState,'IDLE') %send RTS from the queue only in idle state
            if ~isempty(CurrentRTSQueue)
                if iscell(CurrentRTSQueue)
                    CurrentOutgoing = CurrentRTSQueue{1};
                else
                    CurrentOutgoing = CurrentRTSQueue(1);
                end;
                if iscell(CurrentOutgoing)
                    CurrentOutgoing = CurrentOutgoing{1};
                end;
                OutgoingPacket{1} = CurrentOutgoing;
            end;
        end;
    end;
    
    if ~(isempty(OutgoingPacket{1}))
        %Transmit
        TxPacket = OutgoingPacket{1};
        
        TimeDiff = etime(clock, StartTime);
        RemTime = SlotRes - rem(TimeDiff, SlotRes);
        TxTime = clock;
        TxTime(6) = round(TxTime(6) + RemTime);
        if TxTime(6) >= 60
            TxTime(6) = TxTime(6) - 60;
            TxTime(5) = TxTime(5) + 1;
            if TxTime(5) >= 60
                TxTime(5) = TxTime(5) - 60;
                TxTime(4) = TxTime(4) + 1;
            end
        end
        TxPacket.TxTime = TxTime(4:6);
        
        %Transmit packet. Data: [TxPacket.Source, TxPacket.DestVec, TxPacket.Type, TxPacket.MsgID]
        Tx2Phy(TxPacket);
        
        OutgoingPacket{1} = {};
        
        LastState = State{1};
        switch (TxPacket.Type)
            case('RTS')
                State{1} = 'WFCTS';
                CTSWait(1) = 2; % wait two slots for CTS
                CTSWaitSource(1) = TxPacket.DestVec;
            case('CTS')
                State{1} = 'WFDATA';
                DataWait(1) = 2; % wait one slot for DATA
                disp(['Going into state: ', State{1}]);
            case('DATA')
                State{1} = 'IDLE'; % in data transmission
                disp(['Going into state: ', State{1}]);
        end;
        disp(['Going from State: ', LastState, ', into state: ', State{1}]);
    end
    
    %wait till the end of the time slot
    TimeDiff = etime(clock, StartTime);
    RemTime = SlotRes - rem(TimeDiff, SlotRes);
    if RemTime<=0
        %sync error!
        disp(['Warnining: Synchronization error !!!!']);
    end
    NextSlotID = SlotID;
    while(SlotID == NextSlotID)
        TimeDiff = etime(clock, StartTime);
        NextSlotID = floor(TimeDiff / SlotRes) + 1;
        pause(0.05);
    end
    
end;

