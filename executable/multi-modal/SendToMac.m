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


function SendToMac( destination, packet2sendMat , msgIDVec ) 

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
outFileName = 'FromMac2Phy.txt';
DESERTreadyFileName = 'MacTx.txt';


if ~any( 1-( size(packet2sendMat)==[3 1] ) ) || ~any( 1-( size(packet2sendMat)==[1 3] ) )  % The size of packet2sendMat is [3 1]

    % Find required technologies
    techUsed = packet2sendMat ~= 0;

    if any( techUsed )
        fout = fopen( [outFileDir '/' outFileName] , 'w' );
        
        for it = 1 : length( packet2sendMat )
            if ~techUsed(it)
                continue
            end
            singleEntry = packet2sendMat( it );
            
            dataOutHex = bin2hex( dec2bin( singleEntry,SINGLEENTRY_FLD.BITS ) );
            
            % The "-1" is for convenience when the MsgIDVec is not needed
            fprintf( fout , '%d,%d,%d,%s\n' , -1 , destination , it , dataOutHex );
        end
        
        fclose( fout );
    end    

else % The size of packet2sendMat is expected to be [ 3 4+N ]
    
    % Find required technologies
    indTechUsed = any( packet2sendMat , 2 );
    
    % Convert each non-zero row of packet2sendMat
    if any( indTechUsed )
        fout = fopen( [outFileDir '/' outFileName] , 'w' );
        
        for it = 1 : size( packet2sendMat , 1 )
            if ~indTechUsed(it)
                continue
            end
            pQueue = packet2sendMat( it,1 );
            pckSize = packet2sendMat( it,2 );
            origSource = packet2sendMat( it,3 );
            pckInd = packet2sendMat( it,4 );
            historyVec = packet2sendMat( it,5:end );
            
            if length(historyVec) ~= 6
                fprintf('Detected historyVec field composed of less than %d bits.\n' , HISTORYVEC_FLD.BITS);
            end
            
            dataOutBin = [ dec2bin(pQueue,PQUEUE_FLD.BITS) dec2bin(pckSize,PCKSIZE_FLD.BITS) dec2bin(origSource,ORIGSOURCE_FLD.BITS) dec2bin(pckInd,PCKIND_FLD.BITS) num2str(historyVec,'%d') ];
            
            dataOutHex = bin2hex( dataOutBin );
            
            fprintf( fout , '%d,%d,%d,%s\n' , msgIDVec(it) , destination , it , dataOutHex );
        end
        
        fclose( fout );
    end
    
end

% Signal that there is a transmission ready
fdes = fopen( [outFileDir '/' DESERTreadyFileName] , 'w' );
fclose(fdes);













