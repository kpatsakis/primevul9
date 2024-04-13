int JBIG2Stream::getChars(int nChars, unsigned char *buffer)
{
    int n, i;

    if (nChars <= 0 || !dataPtr) {
        return 0;
    }
    if (dataEnd - dataPtr < nChars) {
        n = (int)(dataEnd - dataPtr);
    } else {
        n = nChars;
    }
    for (i = 0; i < n; ++i) {
        buffer[i] = *dataPtr++ ^ 0xff;
    }
    return n;
}