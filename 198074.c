int RateTransposer::isEmpty() const
{
    int res;

    res = FIFOProcessor::isEmpty();
    if (res == 0) return 0;
    return inputBuffer.isEmpty();
}