void TDStretch::clearMidBuffer()
{
    memset(pMidBuffer, 0, channels * sizeof(SAMPLETYPE) * overlapLength);
}