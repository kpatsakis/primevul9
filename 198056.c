FIFOSampleBuffer::~FIFOSampleBuffer()
{
    delete[] bufferUnaligned;
    bufferUnaligned = NULL;
    buffer = NULL;
}