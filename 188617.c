unsigned int JBIG2HuffmanDecoder::readBit()
{
    if (bufLen == 0) {
        buf = str->getChar();
        ++byteCounter;
        bufLen = 8;
    }
    --bufLen;
    return (buf >> bufLen) & 1;
}