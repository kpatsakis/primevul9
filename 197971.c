uint FIFOSampleBuffer::receiveSamples(SAMPLETYPE *output, uint maxSamples)
{
    uint num;

    num = (maxSamples > samplesInBuffer) ? samplesInBuffer : maxSamples;

    memcpy(output, ptrBegin(), channels * sizeof(SAMPLETYPE) * num);
    return receiveSamples(num);
}