int JBIG2MMRDecoder::getWhiteCode()
{
    const CCITTCode *p;
    unsigned int code;

    if (bufLen == 0) {
        buf = str->getChar() & 0xff;
        bufLen = 8;
        ++nBytesRead;
        ++byteCounter;
    }
    while (true) {
        if (bufLen >= 11 && ((buf >> (bufLen - 7)) & 0x7f) == 0) {
            if (bufLen <= 12) {
                code = buf << (12 - bufLen);
            } else {
                code = buf >> (bufLen - 12);
            }
            p = &whiteTab1[code & 0x1f];
        } else {
            if (bufLen <= 9) {
                code = buf << (9 - bufLen);
            } else {
                code = buf >> (bufLen - 9);
            }
            p = &whiteTab2[code & 0x1ff];
        }
        if (p->bits > 0 && p->bits <= (int)bufLen) {
            bufLen -= p->bits;
            return p->n;
        }
        if (bufLen >= 12) {
            break;
        }
        buf = (buf << 8) | (str->getChar() & 0xff);
        bufLen += 8;
        ++nBytesRead;
        ++byteCounter;
    }
    error(errSyntaxError, str->getPos(), "Bad white code in JBIG2 MMR stream");
    // eat a bit and return a positive number so that the caller doesn't
    // go into an infinite loop
    --bufLen;
    return 1;
}