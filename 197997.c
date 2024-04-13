void SoundTouch::flush()
{
    int i;
    int numStillExpected;
    SAMPLETYPE *buff = new SAMPLETYPE[128 * channels];

    // how many samples are still expected to output
    numStillExpected = (int)((long)(samplesExpectedOut + 0.5) - samplesOutput);
    if (numStillExpected < 0) numStillExpected = 0;

    memset(buff, 0, 128 * channels * sizeof(SAMPLETYPE));
    // "Push" the last active samples out from the processing pipeline by
    // feeding blank samples into the processing pipeline until new, 
    // processed samples appear in the output (not however, more than 
    // 24ksamples in any case)
    for (i = 0; (numStillExpected > (int)numSamples()) && (i < 200); i ++)
    {
        putSamples(buff, 128);
    }

    adjustAmountOfSamples(numStillExpected);

    delete[] buff;

    // Clear input buffers
 //   pRateTransposer->clearInput();
    pTDStretch->clearInput();
    // yet leave the output intouched as that's where the
    // flushed samples are!
}