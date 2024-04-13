void JBIG2MMRDecoder::skipTo(unsigned int length)
{
    int n = str->discardChars(length - nBytesRead);
    nBytesRead += n;
    byteCounter += n;
}