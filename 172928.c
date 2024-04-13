preload_image (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
  bmp_source_ptr source = (bmp_source_ptr) sinfo;
  register FILE *infile = source->pub.input_file;
  register JSAMPROW out_ptr;
  JSAMPARRAY image_ptr;
  JDIMENSION row;
  cd_progress_ptr progress = (cd_progress_ptr) cinfo->progress;

  /* Read the data into a virtual array in input-file row order. */
  for (row = 0; row < cinfo->image_height; row++) {
    if (progress != NULL) {
      progress->pub.pass_counter = (long) row;
      progress->pub.pass_limit = (long) cinfo->image_height;
      (*progress->pub.progress_monitor) ((j_common_ptr) cinfo);
    }
    image_ptr = (*cinfo->mem->access_virt_sarray)
      ((j_common_ptr) cinfo, source->whole_image,
       row, (JDIMENSION) 1, TRUE);
    out_ptr = image_ptr[0];
    if (fread(out_ptr, 1, source->row_width, infile) != source->row_width) {
      if (feof(infile))
        ERREXIT(cinfo, JERR_INPUT_EOF);
      else
        ERREXIT(cinfo, JERR_FILE_READ);
    }
  }
  if (progress != NULL)
    progress->completed_extra_passes++;

  /* Set up to read from the virtual array in top-to-bottom order */
  switch (source->bits_per_pixel) {
  case 8:
    source->pub.get_pixel_rows = get_8bit_row;
    break;
  case 24:
    source->pub.get_pixel_rows = get_24bit_row;
    break;
  case 32:
    source->pub.get_pixel_rows = get_32bit_row;
    break;
  default:
    ERREXIT(cinfo, JERR_BMP_BADDEPTH);
  }
  source->source_row = cinfo->image_height;

  /* And read the first row */
  return (*source->pub.get_pixel_rows) (cinfo, sinfo);
}