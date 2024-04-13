void TDStretch::overlapMono(SAMPLETYPE *pOutput, const SAMPLETYPE *pInput) const
{
    int i;
    SAMPLETYPE m1, m2;

    m1 = (SAMPLETYPE)0;
    m2 = (SAMPLETYPE)overlapLength;

    for (i = 0; i < overlapLength ; i ++) 
    {
        pOutput[i] = (pInput[i] * m1 + pMidBuffer[i] * m2 ) / overlapLength;
        m1 += 1;
        m2 -= 1;
    }
}