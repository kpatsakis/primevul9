void JBIG2Stream::readExtensionSeg(unsigned int length)
{
    // skip the segment
    byteCounter += curStr->discardChars(length);
}