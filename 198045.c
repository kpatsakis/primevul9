void TDStretch::setParameters(int aSampleRate, int aSequenceMS, 
                              int aSeekWindowMS, int aOverlapMS)
{
    // accept only positive parameter values - if zero or negative, use old values instead
    if (aSampleRate > 0)
    {
        if (aSampleRate > 192000) ST_THROW_RT_ERROR("Error: Excessive samplerate");
        this->sampleRate = aSampleRate;
    }

    if (aOverlapMS > 0) this->overlapMs = aOverlapMS;

    if (aSequenceMS > 0)
    {
        this->sequenceMs = aSequenceMS;
        bAutoSeqSetting = false;
    } 
    else if (aSequenceMS == 0)
    {
        // if zero, use automatic setting
        bAutoSeqSetting = true;
    }

    if (aSeekWindowMS > 0) 
    {
        this->seekWindowMs = aSeekWindowMS;
        bAutoSeekSetting = false;
    } 
    else if (aSeekWindowMS == 0) 
    {
        // if zero, use automatic setting
        bAutoSeekSetting = true;
    }

    calcSeqParameters();

    calculateOverlapLength(overlapMs);

    // set tempo to recalculate 'sampleReq'
    setTempo(tempo);
}