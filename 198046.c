void FIFOSampleBuffer::putSamples(uint nSamples)
{
    uint req;

    req = samplesInBuffer + nSamples;
    ensureCapacity(req);
    samplesInBuffer += nSamples;
}