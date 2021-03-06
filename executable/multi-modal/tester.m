% Tester for SendToMac and RecvFromMac

%% Set global variables
clear

global  SOURCEID_FLD  PQUEUE_FLD  PCKSIZE_FLD  ORIGSOURCE_FLD  PCKIND_FLD  HISTORYVEC_FLD  SINGLEENTRY_FLD

%%% USE THIS if you want maximum compression
% PQUEUE_FLD      = struct( 'BITS',20 , 'OFFSET',SOURCEID_FLD.OFFSET+SOURCEID_FLD.BITS );
% PCKSIZE_FLD     = struct( 'BITS',14 , 'OFFSET',PQUEUE_FLD.OFFSET+PQUEUE_FLD.BITS );
% ORIGSOURCE_FLD  = struct( 'BITS',03 , 'OFFSET',PCKSIZE_FLD.OFFSET+PCKSIZE_FLD.BITS );
% PCKIND_FLD      = struct( 'BITS',09 , 'OFFSET',ORIGSOURCE_FLD.OFFSET+ORIGSOURCE_FLD.BITS );
% HISTORYVEC_FLD  = struct( 'BITS',06 , 'OFFSET',PCKIND_FLD.OFFSET+PCKIND_FLD.BITS );
% SINGLEENTRY_FLD = struct( 'BITS',20 , 'OFFSET',SOURCEID_FLD.OFFSET+SOURCEID_FLD.BITS );

%%% USE THIS if you want an integer number of bytes for each field
PQUEUE_FLD      = struct( 'BITS',24 , 'OFFSET',0 );
PCKSIZE_FLD     = struct( 'BITS',16 , 'OFFSET',PQUEUE_FLD.OFFSET+PQUEUE_FLD.BITS );
ORIGSOURCE_FLD  = struct( 'BITS',08 , 'OFFSET',PCKSIZE_FLD.OFFSET+PCKSIZE_FLD.BITS );
PCKIND_FLD      = struct( 'BITS',16 , 'OFFSET',ORIGSOURCE_FLD.OFFSET+ORIGSOURCE_FLD.BITS );
HISTORYVEC_FLD  = struct( 'BITS',08 , 'OFFSET',PCKIND_FLD.OFFSET+PCKIND_FLD.BITS );
SINGLEENTRY_FLD = struct( 'BITS',24 , 'OFFSET',0 );

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
pktnum = 666;


formFiletoRecv( source , phynum , pktnum , [pQueue,0,0] );

rxMat = RecvFromPhy()


%% Test reception from PHY where there is only one pQueue in the file and MULTIPLE files
source1 = 2;
phynum1 = 1;
pQueue1 = 143950;
pkntum1 = 444;

source2 = 5;
phynum2 = 3;
pQueue2 = 333333;
pktnum2 = 777;

formFiletoRecv( source1 , phynum1 , pkntum1 , [pQueue1,0,0] );
formFiletoRecv( source2 , phynum2 , pktnum2 , [0,0,pQueue2] );

rxMat = RecvFromPhy()


%% Test reception from PHY where there is a full matrix line entry in the file

source = 1;
phynum = 3;
pktnum = 9999;

M3 = [  0    0  0  0 0 0 0 0 0 0 ;
        0    0  0  0 0 0 0 0 0 0 ;
       25 1540  4 35 0 0 0 1 0 1   ] ;

formFiletoRecv( source , phynum , pktnum , M3 );

rxMat = RecvFromPhy()


%% Test reception from PHY where there is a full matrix line entry in the file and multiple files

source1 = 1;
phynum1 = 3;
M1 = [  0    0  0  0 0 0 0 0 0 0 ;
        0    0  0  0 0 0 0 0 0 0 ;
       11  222  6 33 1 1 1 0 0 0   ] ;
pktnum1 = 555;

source2 = 6;
phynum2 = 2;
M2 = [  0    0  0  0 0 0 0 0 0 0 ;
       25 1540  4 35 0 0 0 1 0 1 ;
        0    0  0  0 0 0 0 0 0 0   ] ;
pktnum2 = 888;

formFiletoRecv( source1 , phynum1 , pktnum1 , M1 );
formFiletoRecv( source2 , phynum2 , pktnum2 , M2 );

rxMat = RecvFromPhy()


%% Test reception from PHY where there are one file with a full matrix entry and another with a single entry

source1 = 1;
phynum1 = 3;
M1 = [  0    0  0  0 0 0 0 0 0 0 ;
        0    0  0  0 0 0 0 0 0 0 ;
       11  222  6 33 1 1 1 0 0 0   ] ;
pktnum1 = round( rand*10000 );

source2 = 5;
phynum2 = 1;
pQueue2 = 333333;
pktnum2 = round( rand*10000 );

source3 = 1;
phynum3 = 2;
M3 = [  0    0  0  0 0 0 0 0 0 0 ;
       10  141  2 10 0 0 1 1 1 0 ;
        0    0  0  0 0 0 0 0 0 0   ] ;
pktnum3 = round( rand*10000 );

source4 = 7;
phynum4 = 2;
pQueue4 = 181818;
pktnum4 = round( rand*10000 );

source5 = 2;
phynum5 = 2;
pQueue5 = 232323;
pktnum5 = round( rand*10000 );

source6 = 3;
phynum6 = 1;
M6 = [ 15  911  3 11 1 1 1 1 1 1 ;
        0    0  0  0 0 0 0 0 0 0 ;
        0    0  0  0 0 0 0 0 0 0  ] ;
pktnum6 = round( rand*10000 );

formFiletoRecv( source1 , phynum1 , pktnum1 , M1 );
formFiletoRecv( source2 , phynum2 , pktnum2 , [pQueue2,0,0] );
formFiletoRecv( source3 , phynum3 , pktnum3 , M3 );
formFiletoRecv( source4 , phynum4 , pktnum4 , [0,pQueue4,0] );
formFiletoRecv( source5 , phynum5 , pktnum5 , [0,pQueue5,0] );
formFiletoRecv( source6 , phynum6 , pktnum6 , M6 );

rxMat = RecvFromPhy()




