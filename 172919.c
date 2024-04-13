put_demapped_rgb (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
                  JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char *bufferptr;
  register int pixval;
  register JSAMPROW ptr;
  register JSAMPROW color_map0 = cinfo->colormap[0];
  register JSAMPROW color_map1 = cinfo->colormap[1];
  register JSAMPROW color_map2 = cinfo->colormap[2];
  register JDIMENSION col;

  ptr = dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
  for (col = cinfo->output_width; col > 0; col--) {
    pixval = GETJSAMPLE(*ptr++);
    PUTPPMSAMPLE(bufferptr, GETJSAMPLE(color_map0[pixval]));
    PUTPPMSAMPLE(bufferptr, GETJSAMPLE(color_map1[pixval]));
    PUTPPMSAMPLE(bufferptr, GETJSAMPLE(color_map2[pixval]));
  }
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}