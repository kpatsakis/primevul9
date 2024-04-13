int RateTransposer::getLatency() const
{
    return (bUseAAFilter) ? pAAFilter->getLength() : 0;
}