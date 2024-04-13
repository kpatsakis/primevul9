put_pixel_rows (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
                JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;

  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}