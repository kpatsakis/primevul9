int JBIG2Stream::lookChar()
{
    if (dataPtr && dataPtr < dataEnd) {
        return (*dataPtr ^ 0xff) & 0xff;
    }
    return EOF;
}