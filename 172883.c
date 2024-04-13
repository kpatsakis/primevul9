start_output_ppm (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;

  /* Emit file header */
  switch (cinfo->out_color_space) {
  case JCS_GRAYSCALE:
    /* emit header for raw PGM format */
    fprintf(dest->pub.output_file, "P5\n%ld %ld\n%d\n",
            (long) cinfo->output_width, (long) cinfo->output_height,
            PPM_MAXVAL);
    break;
  case JCS_RGB:
  case JCS_EXT_RGB:
  case JCS_EXT_RGBX:
  case JCS_EXT_BGR:
  case JCS_EXT_BGRX:
  case JCS_EXT_XBGR:
  case JCS_EXT_XRGB:
  case JCS_EXT_RGBA:
  case JCS_EXT_BGRA:
  case JCS_EXT_ABGR:
  case JCS_EXT_ARGB:
  case JCS_CMYK:
    /* emit header for raw PPM format */
    fprintf(dest->pub.output_file, "P6\n%ld %ld\n%d\n",
            (long) cinfo->output_width, (long) cinfo->output_height,
            PPM_MAXVAL);
    break;
  default:
    ERREXIT(cinfo, JERR_PPM_COLORSPACE);
  }
}