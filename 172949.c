put_rgb (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
         JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char *bufferptr;
  register JSAMPROW ptr;
  register JDIMENSION col;
  register int rindex = rgb_red[cinfo->out_color_space];
  register int gindex = rgb_green[cinfo->out_color_space];
  register int bindex = rgb_blue[cinfo->out_color_space];
  register int ps = rgb_pixelsize[cinfo->out_color_space];

  ptr = dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
  for (col = cinfo->output_width; col > 0; col--) {
    PUTPPMSAMPLE(bufferptr, ptr[rindex]);
    PUTPPMSAMPLE(bufferptr, ptr[gindex]);
    PUTPPMSAMPLE(bufferptr, ptr[bindex]);
    ptr += ps;
  }
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}