get_scaled_gray_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-byte-format PGM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  register JSAMPROW ptr;
  register U_CHAR *bufferptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;

  if (! ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  ptr = source->pub.buffer[0];
  bufferptr = source->iobuffer;
  for (col = cinfo->image_width; col > 0; col--) {
    *ptr++ = rescale[UCH(*bufferptr++)];
  }
  return 1;
}