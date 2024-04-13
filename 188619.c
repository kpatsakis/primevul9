int JBIG2Stream::getChar()
{
    if (dataPtr && dataPtr < dataEnd) {
        return (*dataPtr++ ^ 0xff) & 0xff;
    }
    return EOF;
}