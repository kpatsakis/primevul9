get_rgb_gray_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-byte-format PPM files with any maxval and
   converting to grayscale */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW ptr;
  register U_CHAR *bufferptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;
  unsigned int maxval = source->maxval;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = source->pub.buffer[0];
  bufferptr = source->iobuffer;
  if (maxval == MAXJSAMPLE) {
    for (col = cinfo->image_width; col > 0; col--) {
      JSAMPLE r = *bufferptr++;
      JSAMPLE g = *bufferptr++;
      JSAMPLE b = *bufferptr++;
      *ptr++ = RGB2GRAY(r, g, b);
    }
  } else {
    for (col = cinfo->image_width; col > 0; col--) {
      JSAMPLE r = rescale[UCH(*bufferptr++)];
      JSAMPLE g = rescale[UCH(*bufferptr++)];
      JSAMPLE b = rescale[UCH(*bufferptr++)];
      *ptr++ = RGB2GRAY(r, g, b);
    }
  }
  return 1;
}