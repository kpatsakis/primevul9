void TDStretch::processSamples()
{
    int ovlSkip;
    int offset = 0;
    int temp;

    /* Removed this small optimization - can introduce a click to sound when tempo setting
       crosses the nominal value
    if (tempo == 1.0f) 
    {
        // tempo not changed from the original, so bypass the processing
        processNominalTempo();
        return;
    }
    */

    // Process samples as long as there are enough samples in 'inputBuffer'
    // to form a processing frame.
    while ((int)inputBuffer.numSamples() >= sampleReq) 
    {
        if (isBeginning == false)
        {
            // apart from the very beginning of the track, 
            // scan for the best overlapping position & do overlap-add
            offset = seekBestOverlapPosition(inputBuffer.ptrBegin());

            // Mix the samples in the 'inputBuffer' at position of 'offset' with the 
            // samples in 'midBuffer' using sliding overlapping
            // ... first partially overlap with the end of the previous sequence
            // (that's in 'midBuffer')
            overlap(outputBuffer.ptrEnd((uint)overlapLength), inputBuffer.ptrBegin(), (uint)offset);
            outputBuffer.putSamples((uint)overlapLength);
            offset += overlapLength;
        }
        else
        {
            // Adjust processing offset at beginning of track by not perform initial overlapping
            // and compensating that in the 'input buffer skip' calculation
            isBeginning = false;
            int skip = (int)(tempo * overlapLength + 0.5);

            #ifdef SOUNDTOUCH_ALLOW_NONEXACT_SIMD_OPTIMIZATION
                #ifdef SOUNDTOUCH_ALLOW_SSE
                // if SSE mode, round the skip amount to value corresponding to aligned memory address
                if (channels == 1)
                {
                    skip &= -4;
                }
                else if (channels == 2)
                {
                    skip &= -2;
                }
                #endif
            #endif
            skipFract -= skip;
            assert(nominalSkip >= -skipFract);
        }

        // ... then copy sequence samples from 'inputBuffer' to output:

        // crosscheck that we don't have buffer overflow...
        if ((int)inputBuffer.numSamples() < (offset + seekWindowLength - overlapLength))
        {
            continue;    // just in case, shouldn't really happen
        }

        // length of sequence
        temp = (seekWindowLength - 2 * overlapLength);
        outputBuffer.putSamples(inputBuffer.ptrBegin() + channels * offset, (uint)temp);

        // Copies the end of the current sequence from 'inputBuffer' to 
        // 'midBuffer' for being mixed with the beginning of the next 
        // processing sequence and so on
        assert((offset + temp + overlapLength) <= (int)inputBuffer.numSamples());
        memcpy(pMidBuffer, inputBuffer.ptrBegin() + channels * (offset + temp), 
            channels * sizeof(SAMPLETYPE) * overlapLength);

        // Remove the processed samples from the input buffer. Update
        // the difference between integer & nominal skip step to 'skipFract'
        // in order to prevent the error from accumulating over time.
        skipFract += nominalSkip;   // real skip size
        ovlSkip = (int)skipFract;   // rounded to integer skip
        skipFract -= ovlSkip;       // maintain the fraction part, i.e. real vs. integer skip
        inputBuffer.receiveSamples((uint)ovlSkip);
    }
}