lineBufferMinY (int y, int minY, int linesInLineBuffer)
{
    return ((y - minY) / linesInLineBuffer) * linesInLineBuffer + minY;
}