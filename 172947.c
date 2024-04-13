get_rgb_cmyk_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-byte-format PPM files with any maxval and
   converting to CMYK */
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
      rgb_to_cmyk(r, g, b, ptr, ptr + 1, ptr + 2, ptr + 3);
      ptr += 4;
    }
  } else {
    for (col = cinfo->image_width; col > 0; col--) {
      JSAMPLE r = rescale[UCH(*bufferptr++)];
      JSAMPLE g = rescale[UCH(*bufferptr++)];
      JSAMPLE b = rescale[UCH(*bufferptr++)];
      rgb_to_cmyk(r, g, b, ptr, ptr + 1, ptr + 2, ptr + 3);
      ptr += 4;
    }
  }
  return 1;
}