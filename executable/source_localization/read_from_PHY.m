%% Function to read files written by DESERT modem's drivers
%  It basically constitute an interface between Matlab and DESERT
%  is should be superflouos to note that you might not put a file with the
%  same file name and wrong content.
% Output a struct wuth fields: 1) HEX string of values and 2) value of
% time in msec since epoch (1/1/1970) and an intger with pck number.

% Author: Nigel Farage

function [CurrentPckSym, PckTime, PckType, pck_n] = read_from_PHY(PckNumHeadMat, PckNumTailMat, NodeInd, ...
    NumHead, NumTail, PckLenType1, PckLenType2, MinSameForSOSSource, NumTxDelay, MaxRxPckLen)

% Obtain ls in actual dir for filename pattern specified
folder = '.';
filename = 's_loc_pck_';
filextension = '_.rcv';
check_init = 'rx_flag_';
check_end  = '_.txt';
ls_dir = dir([folder '/' filename '*' filextension]);
check_flag = dir([folder '/' check_init '*' check_end]);
% format is: s_loc_pck_*_.rcv

CurrentPckSym = [];
pck_n = 0;
PckType = 0;
PckTime = 0;
N = size(PckNumHeadMat, 1);

if ~isempty(check_flag) % check flags exist so read them
    
    % Find minimum
    pck_n = Inf;
    for i=1:length(check_flag)
        check_filename = check_flag(i).name;
        filename_str = textscan( check_filename, '%s', 'Delimiter' , '_' );
        n = filename_str{1}{3}; %%% Be aware of where th pck num is!!!
        if n < pck_n
            pck_n = n;
            CurrentCheckName = check_filename;
        end
    end
    
    delete(CurrentCheckName);
    
    filename = ['s_loc_pck_', pck_n, '_.rcv'];
    
    f_id = fopen(filename, 'r');
    if f_id ~= -1
        text_read = fscanf( f_id , '%s' );
        text_read_scan = textscan(text_read , '%s' , 'Delimiter' , ',');
        
        Pck = text_read_scan{1}{1};
        
        PckTime = str2num(text_read_scan{1}{2})/1000;
        
        RxPck = [];
        for ind = 1:2: length(Pck)
            CurrentSym = Pck(ind:ind+1);
            if ~(any(strfind(CurrentSym, '-'))) && ~(any(strfind(CurrentSym, '+'))) && ~(any(strfind(CurrentSym, '.')))
                RxPck = [RxPck, base2dec(CurrentSym, 16)];
            end
        end
        
        RxPck = RxPck(1: min([length(RxPck), MaxRxPckLen]));
        
        %look for SOS source
        diff = RxPck(2: end) - RxPck(1: end-1);
        NumZero = length(find(diff == 0));
        if NumZero > MinSameForSOSSource
            PckType = 1;
        else
            %CheckPckType;
            PckType = 2;
            CurrentPckSym = RxPck;
        end
    end
    
    fclose(f_id);
    
    delete(filename);
end



end
