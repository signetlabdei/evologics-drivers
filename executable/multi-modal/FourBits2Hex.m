% outHex = FourBits2Hex( binBits )
% 
% Converts string binBits of 4 1/0 chars to a hex digit
function outHex = FourBits2Hex( binBits )

if ~ischar( binBits ) || length(binBits)~=4 || (sum(binBits=='1')+sum(binBits=='0'))~=4
    error('FourBits2Hex::The input variable binBits should be a string of four 1/0 chars.');
end


switch binBits 
    case '0000'
        outHex = '0';
    case '0001'
        outHex = '1';
    case '0010'
        outHex = '2';
    case '0011'
        outHex = '3';
    case '0100'
        outHex = '4';
    case '0101'
        outHex = '5';
    case '0110'
        outHex = '6';
    case '0111'
        outHex = '7';
    case '1000'
        outHex = '8';
    case '1001'
        outHex = '9';
    case '1010'
        outHex = 'A';
    case '1011'
        outHex = 'B';
    case '1100'
        outHex = 'C';
    case '1101'
        outHex = 'D';
    case '1110'
        outHex = 'E';
    case '1111'
        outHex = 'F';
    otherwise
        error('TwoBinBytes2Hex::Something very wrong has happened');
end