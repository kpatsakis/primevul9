double TDStretch::calcCrossCorr(const short *mixingPos, const short *compare, double &norm)
{
    long corr;
    unsigned long lnorm;
    int i;

    corr = lnorm = 0;
    // Same routine for stereo and mono. For stereo, unroll loop for better
    // efficiency and gives slightly better resolution against rounding. 
    // For mono it same routine, just  unrolls loop by factor of 4
    for (i = 0; i < channels * overlapLength; i += 4) 
    {
        corr += (mixingPos[i] * compare[i] + 
                 mixingPos[i + 1] * compare[i + 1]) >> overlapDividerBitsNorm;  // notice: do intermediate division here to avoid integer overflow
        corr += (mixingPos[i + 2] * compare[i + 2] + 
                mixingPos[i + 3] * compare[i + 3]) >> overlapDividerBitsNorm;
        lnorm += (mixingPos[i] * mixingPos[i] + 
                mixingPos[i + 1] * mixingPos[i + 1]) >> overlapDividerBitsNorm; // notice: do intermediate division here to avoid integer overflow
        lnorm += (mixingPos[i + 2] * mixingPos[i + 2] + 
                mixingPos[i + 3] * mixingPos[i + 3]) >> overlapDividerBitsNorm;
    }

    if (lnorm > maxnorm)
    {
        // modify 'maxnorm' inside critical section to avoid multi-access conflict if in OpenMP mode
        #pragma omp critical
        if (lnorm > maxnorm)
        {
            maxnorm = lnorm;
        }
    }
    // Normalize result by dividing by sqrt(norm) - this step is easiest 
    // done using floating point operation
    norm = (double)lnorm;
    return (double)corr / sqrt((norm < 1e-9) ? 1.0 : norm);
}