bool JBIG2Stream::readUWord(unsigned int *x)
{
    int c0, c1;

    if ((c0 = curStr->getChar()) == EOF || (c1 = curStr->getChar()) == EOF) {
        return false;
    }
    byteCounter += 2;
    *x = (unsigned int)((c0 << 8) | c1);
    return true;
}