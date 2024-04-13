unsigned int JBIG2MMRDecoder::get24Bits()
{
    while (bufLen < 24) {
        buf = (buf << 8) | (str->getChar() & 0xff);
        bufLen += 8;
        ++nBytesRead;
        ++byteCounter;
    }
    return (buf >> (bufLen - 24)) & 0xffffff;
}