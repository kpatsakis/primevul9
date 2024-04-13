int LibRaw::thumbOK(INT64 maxsz)
{
  if (!ID.input)
    return 0;
  if (!ID.toffset && !(imgdata.thumbnail.tlength > 0 &&
                       load_raw == &LibRaw::broadcom_load_raw) // RPi
  )
    return 0;
  INT64 fsize = ID.input->size();
  if (fsize > 0x7fffffffU)
    return 0; // No thumb for raw > 2Gb
  int tsize = 0;
  int tcol = (T.tcolors > 0 && T.tcolors < 4) ? T.tcolors : 3;
  if (write_thumb == &LibRaw::jpeg_thumb)
    tsize = T.tlength;
  else if (write_thumb == &LibRaw::ppm_thumb)
    tsize = tcol * T.twidth * T.theight;
  else if (write_thumb == &LibRaw::ppm16_thumb)
    tsize = tcol * T.twidth * T.theight *
            ((imgdata.params.raw_processing_options &
              LIBRAW_PROCESSING_USE_PPM16_THUMBS)
                 ? 2
                 : 1);
#ifdef USE_X3FTOOLS
  else if (write_thumb == &LibRaw::x3f_thumb_loader)
  {
    tsize = x3f_thumb_size();
  }
#endif
  else // Kodak => no check
    tsize = 1;
  if (tsize < 0)
    return 0;
  if (maxsz > 0 && tsize > maxsz)
    return 0;
  return (tsize + ID.toffset <= fsize) ? 1 : 0;
}