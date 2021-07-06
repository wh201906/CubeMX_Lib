L = 32;
% hannWindowS = hann(L, 'symmetric')
hannWindowP = hann(L, 'periodic').'
% flatWindowS = flattopwin(L, 'symmetric')
flatWindowP = flattopwin(L, 'periodic').'
hammingWindowP = hamming(L, 'periodic').'
blackmanWindowP = blackman(L, 'periodic').'
triangWindowP = triang(L).'

% wvtool(hannWindowP)
% wvtool(flatWindowP)
% wvtool(blackmanWindowP)
