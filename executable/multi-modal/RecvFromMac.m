% rxMat = RecvFromMac()
% 
% Processes files from DESERT to be used in MATLAB code for the mulrtimodal experiment.
% 
% Input:
%   - none (will check for PhyRx*.txt files)
%
% Output:
%   - rxMat, which can be either of the following
%       1) a matrix of 3x(4+N), the data in the Xth row is:
%           [PQueue, PckSize, OrigSource, PckInd, HistoryVec]   where
%       2) a 3x1 vector, where the Xth row contains an element which should be represented by 20 bits
%     The format of rxMat depends on the file read from input.


function rxMat = RecvFromMac()

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

rxFiles = dir( [ outFileDir '/' fileHeadTag1 '*' fileTailTag1 ] );

% If no file found, just return an all-zero matrix
%%% TODO: we should just send back something simpler, like a -1 or a "false"
if isempty( rxFiles )
    rxMat = {};
    return;
else
    rxMat = cell( length( rxFiles ) , 1 );
end

    


for ff = 1 : length( rxFiles )
    fileTag = rxFiles(ff).name( length(fileHeadTag1)+1 : strfind( rxFiles(ff).name , fileTailTag1 )-1 );
    
    readFileName = [outFileDir '/' fileHeadTag2 fileTag fileTailTag2];
    fout = fopen( readFileName , 'r' );
    textRead = fscanf( fout , '%s' );
    fclose(fout);
    delete( [outFileDir '/' fileHeadTag1 fileTag fileTailTag1] );
    delete( [outFileDir '/' fileHeadTag2 fileTag fileTailTag2] );
    
    % Process read string to find sourceID and hex data
    textRead_scan = textscan( textRead , '%s' , 'Delimiter' , ',' );
    sourceID = str2double( textRead_scan{1}{1} );
    dataRead = textRead_scan{1}{2};
    
    % Convert to binary stream
    dataRead_bin = hex2bin( dataRead );
    
    if length(dataRead_bin) == 24 % If we read only 3 bytes + the sourceID, act accordingly
        
        % Extract source ID and single pQueue entry
        pQueue = bin2dec( dataRead_bin( SINGLEENTRY_FLD.OFFSET + (1:SINGLEENTRY_FLD.BITS) ) );
        
        rxMat{ff} = [sourceID pQueue];
        
    else % We read more than 3 bytes + the sourceID, so this is a full matrix line
        
        % Prepare output matrix
        rxMat{ff} = zeros(3 , 11);
        
        % Extract remaining parameters
        pQueue      = bin2dec( dataRead_bin( PQUEUE_FLD.OFFSET + (1:PQUEUE_FLD.BITS) ) );
        pckSize     = bin2dec( dataRead_bin( PCKSIZE_FLD.OFFSET + (1:PCKSIZE_FLD.BITS) ) );
        origSource  = bin2dec( dataRead_bin( ORIGSOURCE_FLD.OFFSET + (1:ORIGSOURCE_FLD.BITS) ) );
        pckInd      = bin2dec( dataRead_bin( PCKIND_FLD.OFFSET + (1:PCKIND_FLD.BITS) ) );
        
        historyVec_tmp = repmat( ' ' , 1 , HISTORYVEC_FLD.BITS*2-1 );
        historyVec_tmp(1:2:end) = dataRead_bin( HISTORYVEC_FLD.OFFSET + (1:HISTORYVEC_FLD.BITS) );
        historyVec = str2num( historyVec_tmp );
        historyVec(1:HISTORYVEC_FLD.BITS-6) = []; 
        
        rxMat{ff}( str2num(fileTag) , : ) = [sourceID pQueue pckSize origSource pckInd historyVec] ;
        
    end
end

