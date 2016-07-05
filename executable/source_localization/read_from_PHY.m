%% Function to read files written by DESERT modem's drivers
%  It basically constitute an interface between Matlab and DESERT
%  is should be superflouos to note that you might not put a file with the
%  same file name and wrong content.
% Output a struct wuth fields: 1) HEX string of values and 2) value of
% time in msec since epoch (1/1/1970) and an intger with pck number.

% Author: Nigel Farage

function [packet, pck_n] = read_from_PHY()

% Obtain ls in actual dir for filename pattern specified
folder = '.';
filename = 's_loc_pck_';
filextension = '_.rcv';
ls_dir = dir([folder '/' filename '*' filextension]);
% format is: s_loc_pck_*_.rcv

packet = struct;
pck_n = 0;

if ~isempty(ls_dir) % files exist so read them
    
    for i=1:length(ls_dir)
        
        filename = ls_dir(i).name;
        filename_s = textscan( filename, '%s', 'Delimiter' , '_' );
        pck_n = filename_s{1}{4}; %%% Be aware of where th pck num is!!!
        
        f_id = fopen(filename, 'r');
        text_read = fscanf( f_id , '%s' );
        text_read_scan = textscan(text_read , '%s' , 'Delimiter' , ',');
        
        data_stream = text_read_scan{1}{1};
        %time = text_read_scan{1}{2};
        
        packet.data = data_stream;
        %packet.time = time;
        
        fclose(f_id);

        delete(filename);
    end
    
end


end
