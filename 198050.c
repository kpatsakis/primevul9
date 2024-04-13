void TDStretch::acceptNewOverlapLength(int newOverlapLength)
{
    int prevOvl;

    assert(newOverlapLength >= 0);
    prevOvl = overlapLength;
    overlapLength = newOverlapLength;

    if (overlapLength > prevOvl)
    {
        delete[] pMidBufferUnaligned;

        pMidBufferUnaligned = new SAMPLETYPE[overlapLength * channels + 16 / sizeof(SAMPLETYPE)];
        // ensure that 'pMidBuffer' is aligned to 16 byte boundary for efficiency
        pMidBuffer = (SAMPLETYPE *)SOUNDTOUCH_ALIGN_POINTER_16(pMidBufferUnaligned);

        clearMidBuffer();
    }
}