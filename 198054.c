TDStretch::TDStretch() : FIFOProcessor(&outputBuffer)
{
    bQuickSeek = false;
    channels = 2;

    pMidBuffer = NULL;
    pMidBufferUnaligned = NULL;
    overlapLength = 0;

    bAutoSeqSetting = true;
    bAutoSeekSetting = true;

    maxnorm = 0;
    maxnormf = 1e8;

    skipFract = 0;

    tempo = 1.0f;
    setParameters(44100, DEFAULT_SEQUENCE_MS, DEFAULT_SEEKWINDOW_MS, DEFAULT_OVERLAP_MS);
    setTempo(1.0f);

    clear();
}