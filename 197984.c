FIFOSampleBuffer::FIFOSampleBuffer(int numChannels)
{
    assert(numChannels > 0);
    sizeInBytes = 0; // reasonable initial value
    buffer = NULL;
    bufferUnaligned = NULL;
    samplesInBuffer = 0;
    bufferPos = 0;
    channels = (uint)numChannels;
    ensureCapacity(32);     // allocate initial capacity 
}