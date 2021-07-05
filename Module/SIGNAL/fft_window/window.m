L = 32;
hannWindowS = hann(L, 'symmetric')
hannWindowP = hann(L, 'periodic')
flatWindowS = flattopwin(L, 'symmetric')
flatWindowP = flattopwin(L, 'periodic')

wvtool(hannWindowP)
wvtool(flatWindowP)