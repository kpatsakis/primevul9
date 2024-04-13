    void setOutPipe(FIFOSamplePipe *pOutput)
    {
        assert(output == NULL);
        assert(pOutput != NULL);
        output = pOutput;
    }