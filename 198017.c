uint FIFOSampleBuffer::getCapacity() const
{
    return sizeInBytes / (channels * sizeof(SAMPLETYPE));
}