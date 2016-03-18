function Tx2Phy(TxPacket)

global NodeNum outFolder
[status,curr_epoch] = system('date +%s');
disp([curr_epoch(1:end-1), ' Transmitting ', TxPacket.Type, ' for Msg: ', num2str(TxPacket.MsgID), ' to node(s) ', num2str(TxPacket.DestVec),' at time : ', num2str(TxPacket.TxTime)]);

if 0
    %-------------------
    %for check
    Pe = 0;
    
    for ind = 1: NodeNum
        if ind ~= TxPacket.Source
            if rand() > Pe
                FileName = ['/ToNode_', num2str(ind), '_FromNode_', num2str(TxPacket.Source)];
                save(strcat(outFolder, FileName), 'TxPacket');
            end
        end
    end
    
    %-------------------
else
    
    Source = dec2bin(TxPacket.Source, 5);
    
    MsgID = dec2bin(TxPacket.MsgID, 10);
    
    DestVec = TxPacket.DestVec;
    DestBits = zeros(1, 5);
    DestBits(DestVec) = 1;
    DestBits = num2str(DestBits);
    DestBits = DestBits(1: 3: end);
    
    SlotID = dec2bin(TxPacket.SlotID, 9);
    
    TxTime = TxPacket.TxTime;
    TxTimeBits = [num2str(TxTime(1)), ',', num2str(TxTime(2)), ',', num2str(TxTime(3)), ','];
    
    Type = TxPacket.Type;
    switch(Type)
        case('RTS')
            TypeBits = '000';
        case('CTS')
            TypeBits = '010';
        case('DATA')
            TypeBits = '100';
    end
    
    TxVec = [Source, DestBits, MsgID, TypeBits, SlotID];
    TxVec = dec2hex(bin2dec(TxVec),8);
    
    TextToPhy = [TxTimeBits, TxVec];
    
    FileName = '/FromMac2Phy.txt';
    fid = fopen(strcat(outFolder,FileName), 'w');
    fprintf(fid,'%s\n', TextToPhy);
    fclose(fid);
    
    FileName = '/MacTx.txt';
    fid = fopen(strcat(outFolder,FileName), 'w');
    fclose(fid);
end

return;
