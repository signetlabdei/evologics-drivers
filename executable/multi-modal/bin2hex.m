function outHex = bin2hex( binBits )

if ~ischar( binBits )
    error('bin2hex::The input variable binBits should be a char string.');
end

inlen = length(binBits);

if (sum(binBits=='1')+sum(binBits=='0'))~=inlen
    error('bin2hex::The input variable binBits should only contain 1/0 chars.');
end

% Reformat as a column vector
binBits = binBits(:);

% Check if heading zeros should be added
intHex = fix(length(binBits)/4);
headZeros = mod( length(binBits) , 4 );
if headZeros ~= 0
    headZeros = 4 - headZeros;
    intHex = intHex+1;
end

% Create 4-bit chunks
chunkMat = reshape( [ repmat('0',headZeros,1) ; binBits ] , 4 , intHex)';

% Convert to hex
outHex = repmat('0',1,intHex);
for ii = 1 : intHex
    outHex(ii) = FourBits2Hex( chunkMat(ii,:) );
end
