inline void TDStretch::overlap(SAMPLETYPE *pOutput, const SAMPLETYPE *pInput, uint ovlPos) const
{
#ifndef USE_MULTICH_ALWAYS
    if (channels == 1)
    {
        // mono sound.
        overlapMono(pOutput, pInput + ovlPos);
    }
    else if (channels == 2)
    {
        // stereo sound
        overlapStereo(pOutput, pInput + 2 * ovlPos);
    } 
    else 
#endif // USE_MULTICH_ALWAYS
    {
        assert(channels > 0);
        overlapMulti(pOutput, pInput + channels * ovlPos);
    }
}