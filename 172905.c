finish_output_bmp (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo)
{
  bmp_dest_ptr dest = (bmp_dest_ptr) dinfo;
  register FILE *outfile = dest->pub.output_file;
  JSAMPARRAY image_ptr;
  register JSAMPROW data_ptr;
  JDIMENSION row;
  register JDIMENSION col;
  cd_progress_ptr progress = (cd_progress_ptr) cinfo->progress;

  if (dest->use_inversion_array) {
    /* Write the header and colormap */
    if (dest->is_os2)
      write_os2_header(cinfo, dest);
    else
      write_bmp_header(cinfo, dest);

    /* Write the file body from our virtual array */
    for (row = cinfo->output_height; row > 0; row--) {
      if (progress != NULL) {
        progress->pub.pass_counter = (long) (cinfo->output_height - row);
        progress->pub.pass_limit = (long) cinfo->output_height;
        (*progress->pub.progress_monitor) ((j_common_ptr) cinfo);
      }
      image_ptr = (*cinfo->mem->access_virt_sarray)
        ((j_common_ptr) cinfo, dest->whole_image, row-1, (JDIMENSION) 1, FALSE);
      data_ptr = image_ptr[0];
      for (col = dest->row_width; col > 0; col--) {
        putc(GETJSAMPLE(*data_ptr), outfile);
        data_ptr++;
      }
    }
    if (progress != NULL)
      progress->completed_extra_passes++;
  }

  /* Make sure we wrote the output file OK */
  fflush(outfile);
  if (ferror(outfile))
    ERREXIT(cinfo, JERR_FILE_WRITE);
}