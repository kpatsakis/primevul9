void bjc_init_tresh(gx_device_bjc_printer *dev, int rnd)
{
    int i=(int)(time(NULL) & 0x0ff);
    float delta=40.64*rnd;
    for(;i>0;i--) bjc_rand(dev);
    for(i=-512; i<512; i++) dev->bjc_treshold[i+512] =
                                (int)(delta * i / 1024.0 + 2040);
}                      /* init treshold array ~rnd% around halfway (127*16) */