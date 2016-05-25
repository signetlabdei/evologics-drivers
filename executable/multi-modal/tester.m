% Tester for SendToMac and RecvFromMac

%% Set global variables
clear

global  SOURCEID_FLD  PQUEUE_FLD  PCKSIZE_FLD  ORIGSOURCE_FLD  PCKIND_FLD  HISTORYVEC_FLD  SINGLEENTRY_FLD

SOURCEID_FLD    = struct( 'BITS',05 , 'OFFSET',0 );
PQUEUE_FLD      = struct( 'BITS',20 , 'OFFSET',SOURCEID_FLD.OFFSET+SOURCEID_FLD.BITS );
PCKSIZE_FLD     = struct( 'BITS',14 , 'OFFSET',PQUEUE_FLD.OFFSET+PQUEUE_FLD.BITS );
ORIGSOURCE_FLD  = struct( 'BITS',03 , 'OFFSET',PCKSIZE_FLD.OFFSET+PCKSIZE_FLD.BITS );
PCKIND_FLD      = struct( 'BITS',09 , 'OFFSET',ORIGSOURCE_FLD.OFFSET+ORIGSOURCE_FLD.BITS );
HISTORYVEC_FLD  = struct( 'BITS',06 , 'OFFSET',PCKIND_FLD.OFFSET+PCKIND_FLD.BITS );
SINGLEENTRY_FLD = struct( 'BITS',20 , 'OFFSET',SOURCEID_FLD.OFFSET+SOURCEID_FLD.BITS );

%%% UNCOMMENT if you want an integer number of bytes for each field
% SOURCEID_FLD    = struct( 'BITS',08 , 'OFFSET',0 );
% PQUEUE_FLD      = struct( 'BITS',24 , 'OFFSET',SOURCEID_FLD.OFFSET+SOURCEID_FLD.BITS );
% PCKSIZE_FLD     = struct( 'BITS',16 , 'OFFSET',PQUEUE_FLD.OFFSET+PQUEUE_FLD.BITS );
% ORIGSOURCE_FLD  = struct( 'BITS',08 , 'OFFSET',PCKSIZE_FLD.OFFSET+PCKSIZE_FLD.BITS );
% PCKIND_FLD      = struct( 'BITS',16 , 'OFFSET',ORIGSOURCE_FLD.OFFSET+ORIGSOURCE_FLD.BITS );
% HISTORYVEC_FLD  = struct( 'BITS',08 , 'OFFSET',PCKIND_FLD.OFFSET+PCKIND_FLD.BITS );
% SINGLEENTRY_FLD = struct( 'BITS',24 , 'OFFSET',SOURCEID_FLD.OFFSET+SOURCEID_FLD.BITS );

%% Test transmission to MAC for a full matrix entry

dest1 = 1;
M1 = [ 25 1540  4 35 0 0 0 1 0 1 ;
        0    0  0  0 0 0 0 0 0 0 ;
       11  554  7 99 1 1 0 1 0 1  ] ;
msgIDVec1 = [114 226 448];

SendToMac(dest1, M1, msgIDVec1);


%% Test transmission to MAC for a single pQueue entry

dest2 = 3;
M2 = [121541 5161 0];
msgIDVec2 = [];

SendToMac(dest2, M2, msgIDVec2);


%% Test reception from PHY where there is only one pQueue in the file
source = 2;
phynum = 1;
pQueue = 143950;

formFiletoRecv( source , phynum , [pQueue,0,0] );

rxMat = RecvFromMac()


%% Test reception from PHY where there is a full matrix line entry in the file

source = 1;
phynum = 3;

M3 = [  0    0  0  0 0 0 0 0 0 0 ;
        0    0  0  0 0 0 0 0 0 0 ;
       25 1540  4 35 0 0 0 1 0 1   ] ;

formFiletoRecv( source , phynum , M3 );

rxMat = RecvFromMac()











