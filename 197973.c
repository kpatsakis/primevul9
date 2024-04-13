int SoundTouch::getSetting(int settingId) const
{
    int temp;

    switch (settingId) 
    {
        case SETTING_USE_AA_FILTER :
            return (uint)pRateTransposer->isAAFilterEnabled();

        case SETTING_AA_FILTER_LENGTH :
            return pRateTransposer->getAAFilter()->getLength();

        case SETTING_USE_QUICKSEEK :
            return (uint)pTDStretch->isQuickSeekEnabled();

        case SETTING_SEQUENCE_MS:
            pTDStretch->getParameters(NULL, &temp, NULL, NULL);
            return temp;

        case SETTING_SEEKWINDOW_MS:
            pTDStretch->getParameters(NULL, NULL, &temp, NULL);
            return temp;

        case SETTING_OVERLAP_MS:
            pTDStretch->getParameters(NULL, NULL, NULL, &temp);
            return temp;

        case SETTING_NOMINAL_INPUT_SEQUENCE :
        {
            int size = pTDStretch->getInputSampleReq();

#ifndef SOUNDTOUCH_PREVENT_CLICK_AT_RATE_CROSSOVER
            if (rate <= 1.0)
            {
                // transposing done before timestretch, which impacts latency
                return (int)(size * rate + 0.5);
            }
#endif
            return size;
        }

        case SETTING_NOMINAL_OUTPUT_SEQUENCE :
        {
            int size = pTDStretch->getOutputBatchSize();

            if (rate > 1.0)
            {
                // transposing done after timestretch, which impacts latency
                return (int)(size / rate + 0.5);
            }
            return size;
        }

        case SETTING_INITIAL_LATENCY:
        {
            double latency = pTDStretch->getLatency();
            int latency_tr = pRateTransposer->getLatency();

#ifndef SOUNDTOUCH_PREVENT_CLICK_AT_RATE_CROSSOVER
            if (rate <= 1.0)
            {
                // transposing done before timestretch, which impacts latency
                latency = (latency + latency_tr) * rate;
            }
            else
#endif
            {
                latency += (double)latency_tr / rate;
            }

            return (int)(latency + 0.5);
        }

        default :
            return 0;
    }
}