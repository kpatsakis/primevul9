start_output_bmp (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo)
{
  bmp_dest_ptr dest = (bmp_dest_ptr) dinfo;

  if (!dest->use_inversion_array) {
    /* Write the header and colormap */
    if (dest->is_os2)
      write_os2_header(cinfo, dest);
    else
      write_bmp_header(cinfo, dest);
  }
}