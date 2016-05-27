function [destination, msgID, ackStatus, ackForTech] = CheckAck()

outFileDir = '.';
fileHeadTag1 = 'ack_msg_';
fileTailTag1 = '.txt';

rxFiles = dir( [ outFileDir '/' fileHeadTag1 '*' fileTailTag1 ] );

if isempty( rxFiles )
    destrination = [];
    msgID = [];
    ackStatus = [];
    ackForTech = [];
    return;
else
    
end
