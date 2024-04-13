void FIFOSampleBuffer::rewind()
{
    if (buffer && bufferPos) 
    {
        memmove(buffer, ptrBegin(), sizeof(SAMPLETYPE) * channels * samplesInBuffer);
        bufferPos = 0;
    }
}