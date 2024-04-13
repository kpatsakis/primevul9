uint SoundTouch::receiveSamples(SAMPLETYPE *output, uint maxSamples)
{
    uint ret = FIFOProcessor::receiveSamples(output, maxSamples);
    samplesOutput += (long)ret;
    return ret;
}