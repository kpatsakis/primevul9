bool JBIG2Stream::readUByte(unsigned int *x)
{
    int c0;

    if ((c0 = curStr->getChar()) == EOF) {
        return false;
    }
    ++byteCounter;
    *x = (unsigned int)c0;
    return true;
}