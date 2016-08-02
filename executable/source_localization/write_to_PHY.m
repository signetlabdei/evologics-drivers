%% Function that lets available in a file what the modem's DESERT drivers
%  needs to transmit
% It basivally constitutes an interface between Matlab and DESERT
% Output the filename it wrote to.
% Inputs are the HEX string of HEX chars to send out and the pck number to
% be inserted in to filename.

% Author: Arabella Sin

function filename_ = write_to_PHY(message, n_pck, TxFlag)

if TxFlag
    filename = 's_loc_pck';
    filextension = '.snd';
    checkname = 'tx_flag_';
    checkextension = '_.txt';
    check_filename = 'tx_flag.txt';
    out_dir = '.';
    
    tx_message = (dec2base(message, 16))';
    tx_message = (tx_message(:))';
    
    % Open file to write with associated file name
    filename_ = [filename '_' num2str(n_pck) '_' filextension];
    fout = fopen( filename_ , 'w' );
    %fout = fopen( [out_dir '/' filename filextension ] , 'w' );
    
    % Write packet to file: packet should be a string of characters
    fprintf(fout , '%s\n' , tx_message);
    fclose(fout);
     
    check_filename = [checkname num2str(n_pck) checkextension];
    check_file = fopen([out_dir '/' check_filename], 'w');
    fprintf(check_file, '%d\n' , n_pck);
    fclose(check_file);
    
end

end