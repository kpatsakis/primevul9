bool JBIG2Stream::readByte(int *x)
{
    int c0;

    if ((c0 = curStr->getChar()) == EOF) {
        return false;
    }
    ++byteCounter;
    *x = c0;
    if (c0 & 0x80) {
        *x |= -1 - 0xff;
    }
    return true;
}