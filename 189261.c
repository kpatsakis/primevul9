int LibRaw::adjust_maximum()
{
    int i;
    ushort real_max;
    float  auto_threshold;

    if(O.adjust_maximum_thr < 0.00001)
        return LIBRAW_SUCCESS;
    else if (O.adjust_maximum_thr > 0.99999)
        auto_threshold = LIBRAW_DEFAULT_ADJUST_MAXIMUM_THRESHOLD;
    else
        auto_threshold = O.adjust_maximum_thr;
        
    
    real_max = C.channel_maximum[0];
    for(i = 1; i< 4; i++)
        if(real_max < C.channel_maximum[i])
            real_max = C.channel_maximum[i];

    if (real_max > 0 && real_max < C.maximum && real_max > C.maximum* auto_threshold)
        {
            C.maximum = real_max;
        }
    return LIBRAW_SUCCESS;
}