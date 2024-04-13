calc_buffer_dimensions_ppm (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;

  if (cinfo->out_color_space == JCS_GRAYSCALE)
    dest->samples_per_row = cinfo->output_width * cinfo->out_color_components;
  else
    dest->samples_per_row = cinfo->output_width * 3;
  dest->buffer_width = dest->samples_per_row * (BYTESPERSAMPLE * sizeof(char));
}