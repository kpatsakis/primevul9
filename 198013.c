SAMPLETYPE *FIFOSampleBuffer::ptrBegin()
{
    assert(buffer);
    return buffer + bufferPos * channels;
}