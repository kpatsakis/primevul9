bool SoundTouch::setSetting(int settingId, int value)
{
    int sampleRate, sequenceMs, seekWindowMs, overlapMs;

    // read current tdstretch routine parameters
    pTDStretch->getParameters(&sampleRate, &sequenceMs, &seekWindowMs, &overlapMs);

    switch (settingId) 
    {
        case SETTING_USE_AA_FILTER :
            // enables / disabless anti-alias filter
            pRateTransposer->enableAAFilter((value != 0) ? true : false);
            return true;

        case SETTING_AA_FILTER_LENGTH :
            // sets anti-alias filter length
            pRateTransposer->getAAFilter()->setLength(value);
            return true;

        case SETTING_USE_QUICKSEEK :
            // enables / disables tempo routine quick seeking algorithm
            pTDStretch->enableQuickSeek((value != 0) ? true : false);
            return true;

        case SETTING_SEQUENCE_MS:
            // change time-stretch sequence duration parameter
            pTDStretch->setParameters(sampleRate, value, seekWindowMs, overlapMs);
            return true;

        case SETTING_SEEKWINDOW_MS:
            // change time-stretch seek window length parameter
            pTDStretch->setParameters(sampleRate, sequenceMs, value, overlapMs);
            return true;

        case SETTING_OVERLAP_MS:
            // change time-stretch overlap length parameter
            pTDStretch->setParameters(sampleRate, sequenceMs, seekWindowMs, value);
            return true;

        default :
            return false;
    }
}