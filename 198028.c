int TransposerBase::transpose(FIFOSampleBuffer &dest, FIFOSampleBuffer &src)
{
    int numSrcSamples = src.numSamples();
    int sizeDemand = (int)((double)numSrcSamples / rate) + 8;
    int numOutput;
    SAMPLETYPE *psrc = src.ptrBegin();
    SAMPLETYPE *pdest = dest.ptrEnd(sizeDemand);

#ifndef USE_MULTICH_ALWAYS
    if (numChannels == 1)
    {
        numOutput = transposeMono(pdest, psrc, numSrcSamples);
    }
    else if (numChannels == 2) 
    {
        numOutput = transposeStereo(pdest, psrc, numSrcSamples);
    } 
    else 
#endif // USE_MULTICH_ALWAYS
    {
        assert(numChannels > 0);
        numOutput = transposeMulti(pdest, psrc, numSrcSamples);
    }
    dest.putSamples(numOutput);
    src.receiveSamples(numSrcSamples);
    return numOutput;
}