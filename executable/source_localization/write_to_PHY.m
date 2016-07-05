%% Function that lets available in a file what the modem's DESERT drivers
%  needs to transmit
% It basivally constitutes an interface between Matlab and DESERT
% Output the filename it wrote to.
% Inputs are the HEX string of HEX chars to send out and the pck number to
% be inserted in to filename.

% Author: Arabella Sin

function filename_ = write_to_PHY(message, n_pck)

filename = 's_loc_pck';
filextension = '.snd';
out_dir = '.';

% Open file to write with associated file name
%fout = fopen( [out_dir '/' filename '_' num2str(n_pck) '_' filextension ] , 'w' );
fout = fopen( [out_dir '/' filename filextension ] , 'w' );

% Write packet to file: packet should be a string of characters
fprintf(fout , '%s\n' , message);
fclose(fout);

%filename_ = [filename '_' num2str(n_pck) '_' filextension];
filename_ = [filename filextension];

check_filename = 'tx_flag.txt';
check_file = fopen([out_dir '/' check_filename], 'w');
fclose(check_file);

end