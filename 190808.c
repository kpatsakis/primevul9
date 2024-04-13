lineBufferMaxY (int y, int minY, int linesInLineBuffer)
{
    return lineBufferMinY (y, minY, linesInLineBuffer) + linesInLineBuffer - 1;
}