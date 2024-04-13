get_text_gray_cmyk_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading text-format PGM files with any maxval and
   converting to CMYK */
{
  ppm_source_ptr source = (ppm_source_ptr) sinfo;
  FILE *infile = source->pub.input_file;
  register JSAMPROW ptr;
  register JSAMPLE *rescale = source->rescale;
  JDIMENSION col;
  unsigned int maxval = source->maxval;

  ptr = source->pub.buffer[0];
  if (maxval == MAXJSAMPLE) {
    for (col = cinfo->image_width; col > 0; col--) {
      JSAMPLE gray = read_pbm_integer(cinfo, infile, maxval);
      rgb_to_cmyk(gray, gray, gray, ptr, ptr + 1, ptr + 2, ptr + 3);
      ptr += 4;
    }
  } else {
    for (col = cinfo->image_width; col > 0; col--) {
      JSAMPLE gray = rescale[read_pbm_integer(cinfo, infile, maxval)];
      rgb_to_cmyk(gray, gray, gray, ptr, ptr + 1, ptr + 2, ptr + 3);
      ptr += 4;
    }
  }
  return 1;
}