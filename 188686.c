bool JBIG2Stream::readLong(int *x)
{
    int c0, c1, c2, c3;

    if ((c0 = curStr->getChar()) == EOF || (c1 = curStr->getChar()) == EOF || (c2 = curStr->getChar()) == EOF || (c3 = curStr->getChar()) == EOF) {
        return false;
    }
    byteCounter += 4;
    *x = ((c0 << 24) | (c1 << 16) | (c2 << 8) | c3);
    if (c0 & 0x80) {
        *x |= -1 - (int)0xffffffff;
    }
    return true;
}