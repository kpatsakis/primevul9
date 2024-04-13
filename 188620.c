unsigned int JBIG2HuffmanDecoder::readBits(unsigned int n)
{
    unsigned int x, mask, nLeft;

    mask = (n == 32) ? 0xffffffff : ((1 << n) - 1);
    if (bufLen >= n) {
        x = (buf >> (bufLen - n)) & mask;
        bufLen -= n;
    } else {
        x = buf & ((1 << bufLen) - 1);
        nLeft = n - bufLen;
        bufLen = 0;
        while (nLeft >= 8) {
            x = (x << 8) | (str->getChar() & 0xff);
            ++byteCounter;
            nLeft -= 8;
        }
        if (nLeft > 0) {
            buf = str->getChar();
            ++byteCounter;
            bufLen = 8 - nLeft;
            x = (x << nLeft) | ((buf >> bufLen) & ((1 << nLeft) - 1));
        }
    }
    return x;
}