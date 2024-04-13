LibRaw:: LibRaw(unsigned int flags)
{
    double aber[4] = {1,1,1,1};
    double gamm[6] = { 0.45,4.5,0,0,0,0 };
    unsigned greybox[4] =  { 0, 0, UINT_MAX, UINT_MAX };
    unsigned cropbox[4] =  { 0, 0, UINT_MAX, UINT_MAX };
#ifdef DCRAW_VERBOSE
    verbose = 1;
#else
    verbose = 0;
#endif
    ZERO(imgdata);
    ZERO(libraw_internal_data);
    ZERO(callbacks);
    callbacks.mem_cb = (flags & LIBRAW_OPIONS_NO_MEMERR_CALLBACK) ? NULL:  &default_memory_callback;
    callbacks.data_cb = (flags & LIBRAW_OPIONS_NO_DATAERR_CALLBACK)? NULL : &default_data_callback;
    memmove(&imgdata.params.aber,&aber,sizeof(aber));
    memmove(&imgdata.params.gamm,&gamm,sizeof(gamm));
    memmove(&imgdata.params.greybox,&greybox,sizeof(greybox));
    memmove(&imgdata.params.cropbox,&cropbox,sizeof(cropbox));
    
    imgdata.params.bright=1;
    imgdata.params.use_camera_matrix=-1;
    imgdata.params.user_flip=-1;
    imgdata.params.user_black=-1;
    imgdata.params.user_sat=-1;
    imgdata.params.user_qual=-1;
    imgdata.params.output_color=1;
    imgdata.params.output_bps=8;
    imgdata.params.use_fuji_rotate=1;
    imgdata.params.exp_shift = 1.0;
    imgdata.params.auto_bright_thr = LIBRAW_DEFAULT_AUTO_BRIGHTNESS_THRESHOLD;
    imgdata.params.adjust_maximum_thr= LIBRAW_DEFAULT_ADJUST_MAXIMUM_THRESHOLD;
    imgdata.params.green_matching = 0;
    imgdata.parent_class = this;
    imgdata.progress_flags = 0;
    tls = new LibRaw_TLS;
    tls->init();
}