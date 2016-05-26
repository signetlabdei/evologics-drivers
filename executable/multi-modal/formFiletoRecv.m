% SendToMac( destination, packet2sendMat ) 
%
% Creates interface files for MATLAB/DESERT for the June 2016 multimodal
% routing experiment
%
% INPUT
%   - Destination - two options - 
%       - number of a single destination node, 
%       - 0 for broadcast.
%   - packet2sendMat - two options - 
%       - matrix of 3X(4+N)
%       - Vector of 3X1
%   - MsgIDVec - two options - 
%       - a vector of 3 integer (double) values.
%       - An empty value ([]).
%
% Each row X in Packet2SendMat with something different than 0 in it 
% (at least one sample), is regarded as need to use technology X. 
% 
% Packet2SendMat can be 2 things:
% 1) a matrix of 3x(4+N), the data in the Xth row is:
%    [PQueue, PckSize, OrigSource, PckInd, HistoryVec]   where
%       - PQueue is an integer which should be represented by 20 bits (3 Bytes)
%       - PckSize is an integer which should be represented by 14 bits (2 Bytes)
%       - OrigSource is an integer which should be represented by 3 bits (1 Bytes)
%       - PckInd is an integer which should be represented by 9 bits (2 Bytes)
%       - HistoryVec is a vector of 6 (0/1) samples which should be represented by 6 bits (1 Byte);  
% 2) a 3x1 vector, where the Xth row contains
%   - an element which should be represented by 20 bits


function formFiletoRecv( source, tech , packet2sendMat ) 

global  SOURCEID_FLD  PQUEUE_FLD  PCKSIZE_FLD  ORIGSOURCE_FLD  PCKIND_FLD  HISTORYVEC_FLD  SINGLEENTRY_FLD


%%% Should comment, must be set by another file
% SOURCEID_FLD    = struct( 'BITS',05 , 'OFFSET',0 );
% PQUEUE_FLD      = struct( 'BITS',20 , 'OFFSET',SOURCEID_FLD.OFFSET+SOURCEID_FLD.BITS );
% PCKSIZE_FLD     = struct( 'BITS',14 , 'OFFSET',PQUEUE_FLD.OFFSET+PQUEUE_FLD.BITS );
% ORIGSOURCE_FLD  = struct( 'BITS',03 , 'OFFSET',PCKSIZE_FLD.OFFSET+PCKSIZE_FLD.BITS );
% PCKIND_FLD      = struct( 'BITS',09 , 'OFFSET',ORIGSOURCE_FLD.OFFSET+ORIGSOURCE_FLD.BITS );
% HISTORYVEC_FLD  = struct( 'BITS',06 , 'OFFSET',PCKIND_FLD.OFFSET+PCKIND_FLD.BITS );
% SINGLEENTRY_FLD = struct( 'BITS',20 , 'OFFSET',SOURCEID_FLD.OFFSET+SOURCEID_FLD.BITS );


outFileDir = '.';

fileHeadTag1 = 'PhyRx';
fileTailTag1 = '.txt';
fileHeadTag2 = 'FromPhy';
fileTailTag2 = 'toMac.txt';

if sum(size(packet2sendMat)==[3 1])==2 || sum(size(packet2sendMat)==[1 3])==2  % The size of packet2sendMat is [3 1]

    singleEntry = packet2sendMat( tech );
    dataOutHex = bin2hex( dec2bin( singleEntry,SINGLEENTRY_FLD.BITS ) );
    
    fout = fopen( [outFileDir '/' fileHeadTag2 num2str(tech) fileTailTag2 ] , 'w' );
    fprintf( fout , '%d,%s\n' , source,dataOutHex );
    fclose( fout );
    


else % The size of packet2sendMat is expected to be [ 3 4+N ]
    %%%TODO: ELSEIF? Any other conditions to check here?
    
    pQueue = packet2sendMat( tech,1 );
    pckSize = packet2sendMat( tech,2 );
    origSource = packet2sendMat( tech,3 );
    pckInd = packet2sendMat( tech,4 );
    historyVec = [ zeros(1,HISTORYVEC_FLD.BITS-6) packet2sendMat( tech,5:end ) ];
    
    dataOutBin = [ dec2bin(pQueue,PQUEUE_FLD.BITS) dec2bin(pckSize,PCKSIZE_FLD.BITS) dec2bin(origSource,ORIGSOURCE_FLD.BITS) dec2bin(pckInd,PCKIND_FLD.BITS) num2str(historyVec,'%d') ];
    
    dataOutHex = bin2hex( dataOutBin );
    
    fout = fopen( [outFileDir '/' fileHeadTag2 num2str(tech) fileTailTag2 ] , 'w' );
    fprintf( fout , '%d,%s\n' , source , dataOutHex );
    fclose( fout );
    
end

fready = fopen( [outFileDir '/' fileHeadTag1 num2str(tech) fileTailTag1] , 'w');
fclose( fready );






