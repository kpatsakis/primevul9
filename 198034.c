void RateTransposer::processSamples(const SAMPLETYPE *src, uint nSamples)
{
    uint count;

    if (nSamples == 0) return;

    // Store samples to input buffer
    inputBuffer.putSamples(src, nSamples);

    // If anti-alias filter is turned off, simply transpose without applying
    // the filter
    if (bUseAAFilter == false) 
    {
        count = pTransposer->transpose(outputBuffer, inputBuffer);
        return;
    }

    assert(pAAFilter);

    // Transpose with anti-alias filter
    if (pTransposer->rate < 1.0f) 
    {
        // If the parameter 'Rate' value is smaller than 1, first transpose
        // the samples and then apply the anti-alias filter to remove aliasing.

        // Transpose the samples, store the result to end of "midBuffer"
        pTransposer->transpose(midBuffer, inputBuffer);

        // Apply the anti-alias filter for transposed samples in midBuffer
        pAAFilter->evaluate(outputBuffer, midBuffer);
    } 
    else  
    {
        // If the parameter 'Rate' value is larger than 1, first apply the
        // anti-alias filter to remove high frequencies (prevent them from folding
        // over the lover frequencies), then transpose.

        // Apply the anti-alias filter for samples in inputBuffer
        pAAFilter->evaluate(midBuffer, inputBuffer);

        // Transpose the AA-filtered samples in "midBuffer"
        pTransposer->transpose(outputBuffer, midBuffer);
    }
}