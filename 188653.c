int JBIG2MMRDecoder::get2DCode()
{
    const CCITTCode *p = nullptr;

    if (bufLen == 0) {
        buf = str->getChar() & 0xff;
        bufLen = 8;
        ++nBytesRead;
        ++byteCounter;
        p = &twoDimTab1[(buf >> 1) & 0x7f];
    } else if (bufLen == 8) {
        p = &twoDimTab1[(buf >> 1) & 0x7f];
    } else if (bufLen < 8) {
        p = &twoDimTab1[(buf << (7 - bufLen)) & 0x7f];
        if (p->bits < 0 || p->bits > (int)bufLen) {
            buf = (buf << 8) | (str->getChar() & 0xff);
            bufLen += 8;
            ++nBytesRead;
            ++byteCounter;
            p = &twoDimTab1[(buf >> (bufLen - 7)) & 0x7f];
        }
    }
    if (p == nullptr || p->bits < 0) {
        error(errSyntaxError, str->getPos(), "Bad two dim code in JBIG2 MMR stream");
        return EOF;
    }
    bufLen -= p->bits;
    return p->n;
}