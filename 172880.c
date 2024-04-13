jinit_write_ppm (j_decompress_ptr cinfo)
{
  ppm_dest_ptr dest;

  /* Create module interface object, fill in method pointers */
  dest = (ppm_dest_ptr)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
                                  sizeof(ppm_dest_struct));
  dest->pub.start_output = start_output_ppm;
  dest->pub.finish_output = finish_output_ppm;
  dest->pub.calc_buffer_dimensions = calc_buffer_dimensions_ppm;

  /* Calculate output image dimensions so we can allocate space */
  jpeg_calc_output_dimensions(cinfo);

  /* Create physical I/O buffer */
  dest->pub.calc_buffer_dimensions (cinfo, (djpeg_dest_ptr) dest);
  dest->iobuffer = (char *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo, JPOOL_IMAGE, dest->buffer_width);

  if (cinfo->quantize_colors || BITS_IN_JSAMPLE != 8 ||
      sizeof(JSAMPLE) != sizeof(char) ||
      (cinfo->out_color_space != JCS_EXT_RGB
#if RGB_RED == 0 && RGB_GREEN == 1 && RGB_BLUE == 2 && RGB_PIXELSIZE == 3
       && cinfo->out_color_space != JCS_RGB
#endif
      )) {
    /* When quantizing, we need an output buffer for colormap indexes
     * that's separate from the physical I/O buffer.  We also need a
     * separate buffer if pixel format translation must take place.
     */
    dest->pub.buffer = (*cinfo->mem->alloc_sarray)
      ((j_common_ptr) cinfo, JPOOL_IMAGE,
       cinfo->output_width * cinfo->output_components, (JDIMENSION) 1);
    dest->pub.buffer_height = 1;
    if (IsExtRGB(cinfo->out_color_space))
      dest->pub.put_pixel_rows = put_rgb;
    else if (cinfo->out_color_space == JCS_CMYK)
      dest->pub.put_pixel_rows = put_cmyk;
    else if (! cinfo->quantize_colors)
      dest->pub.put_pixel_rows = copy_pixel_rows;
    else if (cinfo->out_color_space == JCS_GRAYSCALE)
      dest->pub.put_pixel_rows = put_demapped_gray;
    else
      dest->pub.put_pixel_rows = put_demapped_rgb;
  } else {
    /* We will fwrite() directly from decompressor output buffer. */
    /* Synthesize a JSAMPARRAY pointer structure */
    dest->pixrow = (JSAMPROW) dest->iobuffer;
    dest->pub.buffer = & dest->pixrow;
    dest->pub.buffer_height = 1;
    dest->pub.put_pixel_rows = put_pixel_rows;
  }

  return (djpeg_dest_ptr) dest;
}