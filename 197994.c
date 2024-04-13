void SoundTouch::putSamples(const SAMPLETYPE *samples, uint nSamples)
{
    if (bSrateSet == false) 
    {
        ST_THROW_RT_ERROR("SoundTouch : Sample rate not defined");
    } 
    else if (channels == 0) 
    {
        ST_THROW_RT_ERROR("SoundTouch : Number of channels not defined");
    }

    // Transpose the rate of the new samples if necessary
    /* Bypass the nominal setting - can introduce a click in sound when tempo/pitch control crosses the nominal value...
    if (rate == 1.0f) 
    {
        // The rate value is same as the original, simply evaluate the tempo changer. 
        assert(output == pTDStretch);
        if (pRateTransposer->isEmpty() == 0) 
        {
            // yet flush the last samples in the pitch transposer buffer
            // (may happen if 'rate' changes from a non-zero value to zero)
            pTDStretch->moveSamples(*pRateTransposer);
        }
        pTDStretch->putSamples(samples, nSamples);
    } 
    */

    // accumulate how many samples are expected out from processing, given the current 
    // processing setting
    samplesExpectedOut += (double)nSamples / ((double)rate * (double)tempo);

#ifndef SOUNDTOUCH_PREVENT_CLICK_AT_RATE_CROSSOVER
    if (rate <= 1.0f) 
    {
        // transpose the rate down, output the transposed sound to tempo changer buffer
        assert(output == pTDStretch);
        pRateTransposer->putSamples(samples, nSamples);
        pTDStretch->moveSamples(*pRateTransposer);
    } 
    else 
#endif
    {
        // evaluate the tempo changer, then transpose the rate up, 
        assert(output == pRateTransposer);
        pTDStretch->putSamples(samples, nSamples);
        pRateTransposer->moveSamples(*pTDStretch);
    }
}