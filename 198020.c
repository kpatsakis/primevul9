    bool verifyNumberOfChannels(int nChannels) const
    {
        if ((nChannels > 0) && (nChannels <= SOUNDTOUCH_MAX_CHANNELS))
        {
            return true;
        }
        ST_THROW_RT_ERROR("Error: Illegal number of channels");
        return false;
    }