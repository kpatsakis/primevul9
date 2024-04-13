numLinesInBuffer (Compressor * compressor)
{
    return compressor? compressor->numScanLines(): 1;
}