put_cmyk (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
          JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char *bufferptr;
  register JSAMPROW ptr;
  register JDIMENSION col;

  ptr = dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
  for (col = cinfo->output_width; col > 0; col--) {
    JSAMPLE r, g, b, c = *ptr++, m = *ptr++, y = *ptr++, k = *ptr++;
    cmyk_to_rgb(c, m, y, k, &r, &g, &b);
    PUTPPMSAMPLE(bufferptr, r);
    PUTPPMSAMPLE(bufferptr, g);
    PUTPPMSAMPLE(bufferptr, b);
  }
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}