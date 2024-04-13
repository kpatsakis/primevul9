void LibRaw:: recycle() 
{
  recycle_datastream();
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
  _exitflag = 0;
#ifdef USE_RAWSPEED
  if(_rawspeed_decoder)
    {
      RawDecoder *d = static_cast<RawDecoder*>(_rawspeed_decoder);
      delete d;
    }
  _rawspeed_decoder = 0;
#endif
  if(_x3f_data)
    {
      x3f_clear(_x3f_data);
      _x3f_data = 0;
    }

  memmgr.cleanup();
  imgdata.thumbnail.tformat = LIBRAW_THUMBNAIL_UNKNOWN;
  imgdata.progress_flags = 0;
  
  tls->init();
}