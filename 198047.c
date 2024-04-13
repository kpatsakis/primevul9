int FIFOSampleBuffer::isEmpty() const
{
    return (samplesInBuffer == 0) ? 1 : 0;
}