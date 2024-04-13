get_text_rgb_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading text-format PPM files with any maxval */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  FILE *infile = source->pub.input_file;
  register JSAMPROW ptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;
  unsigned int maxval = source->maxval;
  register int rindex = rgb_red[cinfo->in_color_space];
  register int gindex = rgb_green[cinfo->in_color_space];
  register int bindex = rgb_blue[cinfo->in_color_space];
  register int aindex = alpha_index[cinfo->in_color_space];
  register int ps = rgb_pixelsize[cinfo->in_color_space];

  ptr = source->pub.buffer[0];
  if (maxval == MAXJSAMPLE) {
    if (aindex >= 0)
      RGB_READ_LOOP(read_pbm_integer(cinfo, infile, maxval),
                    ptr[aindex] = 0xFF;)
    else
      RGB_READ_LOOP(read_pbm_integer(cinfo, infile, maxval),)
  } else {
    if (aindex >= 0)
      RGB_READ_LOOP(rescale[read_pbm_integer(cinfo, infile, maxval)],
                    ptr[aindex] = 0xFF;)
    else
      RGB_READ_LOOP(rescale[read_pbm_integer(cinfo, infile, maxval)],)
  }
  return 1;
}