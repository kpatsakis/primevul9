offsetInLineBufferTable (const vector<size_t> &bytesPerLine,
                         int scanline1, int scanline2,
                         int linesInLineBuffer,
                         vector<size_t> &offsetInLineBuffer)
{
    offsetInLineBuffer.resize (bytesPerLine.size());

    size_t offset = 0;

    for (int i = scanline1; i <= scanline2; ++i)
    {
        if (i % linesInLineBuffer == 0)
            offset = 0;

        offsetInLineBuffer[i] = offset;
        offset += bytesPerLine[i];
    }
}