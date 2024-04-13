finish_output_ppm (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo)
{
  /* Make sure we wrote the output file OK */
  fflush(dinfo->output_file);
  if (ferror(dinfo->output_file))
    ERREXIT(cinfo, JERR_FILE_WRITE);
}