bool JBIG2Stream::readULong(unsigned int *x)
{
    int c0, c1, c2, c3;

    if ((c0 = curStr->getChar()) == EOF || (c1 = curStr->getChar()) == EOF || (c2 = curStr->getChar()) == EOF || (c3 = curStr->getChar()) == EOF) {
        return false;
    }
    byteCounter += 4;
    *x = (unsigned int)((c0 << 24) | (c1 << 16) | (c2 << 8) | c3);
    return true;
}