int TDStretch::seekBestOverlapPosition(const SAMPLETYPE *refPos)
{
    if (bQuickSeek) 
    {
        return seekBestOverlapPositionQuick(refPos);
    }
    else 
    {
        return seekBestOverlapPositionFull(refPos);
    }
}