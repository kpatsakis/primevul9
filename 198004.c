uint FIFOSampleBuffer::receiveSamples(uint maxSamples)
{
    if (maxSamples >= samplesInBuffer)
    {
        uint temp;

        temp = samplesInBuffer;
        samplesInBuffer = 0;
        return temp;
    }

    samplesInBuffer -= maxSamples;
    bufferPos += maxSamples;

    return maxSamples;
}