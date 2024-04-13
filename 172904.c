get_rgb_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-byte-format PPM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW ptr;
  register U_CHAR *bufferptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;
  unsigned int maxval = source->maxval;
  register int rindex = rgb_red[cinfo->in_color_space];
  register int gindex = rgb_green[cinfo->in_color_space];
  register int bindex = rgb_blue[cinfo->in_color_space];
  register int aindex = alpha_index[cinfo->in_color_space];
  register int ps = rgb_pixelsize[cinfo->in_color_space];

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = source->pub.buffer[0];
  bufferptr = source->iobuffer;
  if (maxval == MAXJSAMPLE) {
    if (aindex >= 0)
      RGB_READ_LOOP(*bufferptr++, ptr[aindex] = 0xFF;)
    else
      RGB_READ_LOOP(*bufferptr++,)
  } else {
    if (aindex >= 0)
      RGB_READ_LOOP(rescale[UCH(*bufferptr++)], ptr[aindex] = 0xFF;)
    else
      RGB_READ_LOOP(rescale[UCH(*bufferptr++)],)
  }
  return 1;
}