void LibRaw:: recycle() 
{
    if(libraw_internal_data.internal_data.input && libraw_internal_data.internal_data.input_internal) 
        { 
            delete libraw_internal_data.internal_data.input; 
            libraw_internal_data.internal_data.input = NULL;
        }
    libraw_internal_data.internal_data.input_internal = 0;
#define FREE(a) do { if(a) { free(a); a = NULL;} }while(0)
            
    FREE(imgdata.image); 
    FREE(imgdata.thumbnail.thumb);
    FREE(libraw_internal_data.internal_data.meta_data);
    FREE(libraw_internal_data.output_data.histogram);
    FREE(libraw_internal_data.output_data.oprof);
    FREE(imgdata.color.profile);
    FREE(imgdata.rawdata.ph1_black);
    FREE(imgdata.rawdata.raw_alloc); 
#undef FREE
    ZERO(imgdata.rawdata);
    ZERO(imgdata.sizes);
    ZERO(imgdata.color);
    ZERO(libraw_internal_data);
    memmgr.cleanup();
    imgdata.thumbnail.tformat = LIBRAW_THUMBNAIL_UNKNOWN;
    imgdata.progress_flags = 0;
    
    tls->init();
}