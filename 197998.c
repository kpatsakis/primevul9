int TDStretch::seekBestOverlapPositionQuick(const SAMPLETYPE *refPos)
{
#define _MIN(a, b)   (((a) < (b)) ? (a) : (b))
#define SCANSTEP    16
#define SCANWIND    8

    int bestOffs;
    int i;
    int bestOffs2;
    float bestCorr, corr;
    float bestCorr2;
    double norm;

    // note: 'float' types used in this function in case that the platform would need to use software-fp

    bestCorr =
    bestCorr2 = -FLT_MAX;
    bestOffs = 
    bestOffs2 = SCANWIND;

    // Scans for the best correlation value by testing each possible position
    // over the permitted range. Look for two best matches on the first pass to
    // increase possibility of ideal match.
    //
    // Begin from "SCANSTEP" instead of SCANWIND to make the calculation
    // catch the 'middlepoint' of seekLength vector as that's the a-priori 
    // expected best match position
    //
    // Roughly:
    // - 15% of cases find best result directly on the first round,
    // - 75% cases find better match on 2nd round around the best match from 1st round
    // - 10% cases find better match on 2nd round around the 2nd-best-match from 1st round
    for (i = SCANSTEP; i < seekLength - SCANWIND - 1; i += SCANSTEP)
    {
        // Calculates correlation value for the mixing position corresponding
        // to 'i'
        corr = (float)calcCrossCorr(refPos + channels*i, pMidBuffer, norm);
        // heuristic rule to slightly favour values close to mid of the seek range
        float tmp = (float)(2 * i - seekLength - 1) / (float)seekLength;
        corr = ((corr + 0.1f) * (1.0f - 0.25f * tmp * tmp));

        // Checks for the highest correlation value
        if (corr > bestCorr)
        {
            // found new best match. keep the previous best as 2nd best match
            bestCorr2 = bestCorr;
            bestOffs2 = bestOffs;
            bestCorr = corr;
            bestOffs = i;
        }
        else if (corr > bestCorr2)
        {
            // not new best, but still new 2nd best match
            bestCorr2 = corr;
            bestOffs2 = i;
        }
    }

    // Scans surroundings of the found best match with small stepping
    int end = _MIN(bestOffs + SCANWIND + 1, seekLength);
    for (i = bestOffs - SCANWIND; i < end; i++)
    {
        if (i == bestOffs) continue;    // this offset already calculated, thus skip

        // Calculates correlation value for the mixing position corresponding
        // to 'i'
        corr = (float)calcCrossCorr(refPos + channels*i, pMidBuffer, norm);
        // heuristic rule to slightly favour values close to mid of the range
        float tmp = (float)(2 * i - seekLength - 1) / (float)seekLength;
        corr = ((corr + 0.1f) * (1.0f - 0.25f * tmp * tmp));

        // Checks for the highest correlation value
        if (corr > bestCorr)
        {
            bestCorr = corr;
            bestOffs = i;
        }
    }

    // Scans surroundings of the 2nd best match with small stepping
    end = _MIN(bestOffs2 + SCANWIND + 1, seekLength);
    for (i = bestOffs2 - SCANWIND; i < end; i++)
    {
        if (i == bestOffs2) continue;    // this offset already calculated, thus skip

        // Calculates correlation value for the mixing position corresponding
        // to 'i'
        corr = (float)calcCrossCorr(refPos + channels*i, pMidBuffer, norm);
        // heuristic rule to slightly favour values close to mid of the range
        float tmp = (float)(2 * i - seekLength - 1) / (float)seekLength;
        corr = ((corr + 0.1f) * (1.0f - 0.25f * tmp * tmp));

        // Checks for the highest correlation value
        if (corr > bestCorr)
        {
            bestCorr = corr;
            bestOffs = i;
        }
    }

    // clear cross correlation routine state if necessary (is so e.g. in MMX routines).
    clearCrossCorrState();

#ifdef SOUNDTOUCH_INTEGER_SAMPLES
    adaptNormalizer();
#endif

    return bestOffs;
}