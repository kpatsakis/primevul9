void FIFOSampleBuffer::putSamples(const SAMPLETYPE *samples, uint nSamples)
{
    memcpy(ptrEnd(nSamples), samples, sizeof(SAMPLETYPE) * nSamples * channels);
    samplesInBuffer += nSamples;
}