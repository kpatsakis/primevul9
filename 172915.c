put_demapped_gray (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
                   JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char *bufferptr;
  register JSAMPROW ptr;
  register JSAMPROW color_map = cinfo->colormap[0];
  register JDIMENSION col;

  ptr = dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
  for (col = cinfo->output_width; col > 0; col--) {
    PUTPPMSAMPLE(bufferptr, GETJSAMPLE(color_map[GETJSAMPLE(*ptr++)]));
  }
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}