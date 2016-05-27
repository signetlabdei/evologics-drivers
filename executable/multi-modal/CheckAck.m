function [msgID, ackStatus, ackForTech] = CheckAck()

outFileDir = '.';
fileHeadTag1 = 'ack_msg_';
fileTailTag1 = '.txt';
separator = '_';

rxFiles = dir( [ outFileDir '/' fileHeadTag1 '*' fileTailTag1 ] );

if isempty( rxFiles )
    msgID = [];
    ackStatus = [];
    ackForTech = [];
    return;
else
    msgID = zeros(length(rxFiles), 1);
    ackStatus = msgID;
    ackForTech = msgID;
    for ff = 1 : length( rxFiles )
        fileTag = rxFiles(ff).name( length(fileHeadTag1)+1 : strfind( rxFiles(ff).name , fileTailTag1 )-1 );
        pos = strfind(fileTag,separator);
        msgID(ff) = str2double(fileTag(1 : pos(1) - 1));
        ackStatus(ff) = str2double(fileTag(pos(1) + 1 : pos(2) - 1));
        ackForTech(ff) = str2double(fileTag(pos(2) + 1 : end));
    end    
end
