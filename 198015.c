void TDStretch::calculateOverlapLength(int aoverlapMs)
{
    int newOvl;

    assert(aoverlapMs >= 0);

    // calculate overlap length so that it's power of 2 - thus it's easy to do
    // integer division by right-shifting. Term "-1" at end is to account for 
    // the extra most significatnt bit left unused in result by signed multiplication 
    overlapDividerBitsPure = _getClosest2Power((sampleRate * aoverlapMs) / 1000.0) - 1;
    if (overlapDividerBitsPure > 9) overlapDividerBitsPure = 9;
    if (overlapDividerBitsPure < 3) overlapDividerBitsPure = 3;
    newOvl = (int)pow(2.0, (int)overlapDividerBitsPure + 1);    // +1 => account for -1 above

    acceptNewOverlapLength(newOvl);

    overlapDividerBitsNorm = overlapDividerBitsPure;

    // calculate sloping divider so that crosscorrelation operation won't 
    // overflow 32-bit register. Max. sum of the crosscorrelation sum without 
    // divider would be 2^30*(N^3-N)/3, where N = overlap length
    slopingDivider = (newOvl * newOvl - 1) / 3;
}