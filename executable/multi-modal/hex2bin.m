function outBin = hex2bin( hexDigits )

if ~ischar( hexDigits )
    error('hex2bin::The input variable hexDigits should be a char string.');
end

if any( 1 - isstrprop( hexDigits , 'xdigit') )
    error('hex2bin::The input variable hexDigits should only contain valid hex chars.');
end

% Create output
outBin = repmat( '0' , 1 , 4*length(hexDigits) );

% Convert to bin
for ii = 1 : length( hexDigits )
    outBin(4*(ii-1) + (1:4)) = dec2bin( hex2dec( hexDigits(ii) ) , 4 );
end

