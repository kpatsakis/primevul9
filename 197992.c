int TDStretch::seekBestOverlapPositionFull(const SAMPLETYPE *refPos) 
{
    int bestOffs;
    double bestCorr;
    int i;
    double norm;

    bestCorr = -FLT_MAX;
    bestOffs = 0;

    // Scans for the best correlation value by testing each possible position
    // over the permitted range.
    bestCorr = calcCrossCorr(refPos, pMidBuffer, norm);
    bestCorr = (bestCorr + 0.1) * 0.75;

    #pragma omp parallel for
    for (i = 1; i < seekLength; i ++) 
    {
        double corr;
        // Calculates correlation value for the mixing position corresponding to 'i'
#ifdef _OPENMP
        // in parallel OpenMP mode, can't use norm accumulator version as parallel executor won't
        // iterate the loop in sequential order
        corr = calcCrossCorr(refPos + channels * i, pMidBuffer, norm);
#else
        // In non-parallel version call "calcCrossCorrAccumulate" that is otherwise same
        // as "calcCrossCorr", but saves time by reusing & updating previously stored 
        // "norm" value
        corr = calcCrossCorrAccumulate(refPos + channels * i, pMidBuffer, norm);
#endif
        // heuristic rule to slightly favour values close to mid of the range
        double tmp = (double)(2 * i - seekLength) / (double)seekLength;
        corr = ((corr + 0.1) * (1.0 - 0.25 * tmp * tmp));

        // Checks for the highest correlation value
        if (corr > bestCorr) 
        {
            // For optimal performance, enter critical section only in case that best value found.
            // in such case repeat 'if' condition as it's possible that parallel execution may have
            // updated the bestCorr value in the mean time
            #pragma omp critical
            if (corr > bestCorr)
            {
                bestCorr = corr;
                bestOffs = i;
            }
        }
    }

#ifdef SOUNDTOUCH_INTEGER_SAMPLES
    adaptNormalizer();
#endif

    // clear cross correlation routine state if necessary (is so e.g. in MMX routines).
    clearCrossCorrState();

    return bestOffs;
}