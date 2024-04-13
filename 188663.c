void JBIG2Stream::readProfilesSeg(unsigned int length)
{
    // skip the segment
    byteCounter += curStr->discardChars(length);
}