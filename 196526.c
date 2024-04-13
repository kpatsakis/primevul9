void LibRaw::fix_after_rawspeed(int bl)
{
  if (load_raw == &LibRaw::lossy_dng_load_raw)
    C.maximum = 0xffff;
  else if (load_raw == &LibRaw::sony_load_raw)
    C.maximum = 0x3ff0;
  else if 
    (
     (load_raw == &LibRaw::sony_arw2_load_raw 
      || (load_raw == &LibRaw::packed_load_raw && !strcasecmp(imgdata.idata.make,"Sony"))
      )
     && bl >= (C.black+C.cblack[0])*2
     )
    {
      C.maximum *=4;
      C.black *=4;
      for(int c=0; c< 4; c++)
        C.cblack[c]*=4;
    }
}