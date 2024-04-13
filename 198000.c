uint SoundTouch::receiveSamples(uint maxSamples)
{
    uint ret = FIFOProcessor::receiveSamples(maxSamples);
    samplesOutput += (long)ret;
    return ret;
}