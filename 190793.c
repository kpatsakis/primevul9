offsetInLineBufferTable (const vector<size_t> &bytesPerLine,
			 int linesInLineBuffer,
			 vector<size_t> &offsetInLineBuffer)
{
    offsetInLineBufferTable (bytesPerLine,
                             0, bytesPerLine.size() - 1,
                             linesInLineBuffer,
                             offsetInLineBuffer);
}