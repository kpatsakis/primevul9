void JBIG2Stream::readEndOfStripeSeg(unsigned int length)
{
    // skip the segment
    byteCounter += curStr->discardChars(length);
}