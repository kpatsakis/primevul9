    void moveSamples(FIFOSamplePipe &other  ///< Other pipe instance where from the receive the data.
         )
    {
        int oNumSamples = other.numSamples();

        putSamples(other.ptrBegin(), oNumSamples);
        other.receiveSamples(oNumSamples);
    };