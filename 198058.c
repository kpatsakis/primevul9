uint FIFOSampleBuffer::adjustAmountOfSamples(uint numSamples)
{
    if (numSamples < samplesInBuffer)
    {
        samplesInBuffer = numSamples;
    }
    return samplesInBuffer;
}