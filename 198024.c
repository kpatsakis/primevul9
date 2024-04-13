    virtual uint receiveSamples(uint maxSamples   ///< Remove this many samples from the beginning of pipe.
                                )
    {
        return output->receiveSamples(maxSamples);
    }