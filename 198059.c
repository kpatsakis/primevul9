    virtual uint receiveSamples(SAMPLETYPE *outBuffer, ///< Buffer where to copy output samples.
                                uint maxSamples                    ///< How many samples to receive at max.
                                )
    {
        return output->receiveSamples(outBuffer, maxSamples);
    }