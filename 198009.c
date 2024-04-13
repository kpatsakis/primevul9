double TDStretch::calcCrossCorrAccumulate(const short *mixingPos, const short *compare, double &norm)
{
    long corr;
    unsigned long lnorm;
    int i;

    // cancel first normalizer tap from previous round
    lnorm = 0;
    for (i = 1; i <= channels; i ++)
    {
        lnorm -= (mixingPos[-i] * mixingPos[-i]) >> overlapDividerBitsNorm;
    }

    corr = 0;
    // Same routine for stereo and mono. For stereo, unroll loop for better
    // efficiency and gives slightly better resolution against rounding. 
    // For mono it same routine, just  unrolls loop by factor of 4
    for (i = 0; i < channels * overlapLength; i += 4) 
    {
        corr += (mixingPos[i] * compare[i] + 
                 mixingPos[i + 1] * compare[i + 1]) >> overlapDividerBitsNorm;  // notice: do intermediate division here to avoid integer overflow
        corr += (mixingPos[i + 2] * compare[i + 2] + 
                 mixingPos[i + 3] * compare[i + 3]) >> overlapDividerBitsNorm;
    }

    // update normalizer with last samples of this round
    for (int j = 0; j < channels; j ++)
    {
        i --;
        lnorm += (mixingPos[i] * mixingPos[i]) >> overlapDividerBitsNorm;
    }

    norm += (double)lnorm;
    if (norm > maxnorm)
    {
        maxnorm = (unsigned long)norm;
    }

    // Normalize result by dividing by sqrt(norm) - this step is easiest 
    // done using floating point operation
    return (double)corr / sqrt((norm < 1e-9) ? 1.0 : norm);
}