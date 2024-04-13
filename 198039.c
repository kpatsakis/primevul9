double TDStretch::calcCrossCorrAccumulate(const float *mixingPos, const float *compare, double &norm)
{
    double corr;
    int i;

    corr = 0;

    // cancel first normalizer tap from previous round
    for (i = 1; i <= channels; i ++)
    {
        norm -= mixingPos[-i] * mixingPos[-i];
    }

    // Same routine for stereo and mono. For Stereo, unroll by factor of 2.
    // For mono it's same routine yet unrollsd by factor of 4.
    for (i = 0; i < channels * overlapLength; i += 4) 
    {
        corr += mixingPos[i] * compare[i] +
                mixingPos[i + 1] * compare[i + 1] +
                mixingPos[i + 2] * compare[i + 2] +
                mixingPos[i + 3] * compare[i + 3];
    }

    // update normalizer with last samples of this round
    for (int j = 0; j < channels; j ++)
    {
        i --;
        norm += mixingPos[i] * mixingPos[i];
    }

    return corr / sqrt((norm < 1e-9 ? 1.0 : norm));
}