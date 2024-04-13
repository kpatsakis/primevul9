SAMPLETYPE *FIFOSampleBuffer::ptrEnd(uint slackCapacity) 
{
    ensureCapacity(samplesInBuffer + slackCapacity);
    return buffer + samplesInBuffer * channels;
}