copy_pixel_rows (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
                 JDIMENSION rows_supplied)
{
  ppm_dest_ptr dest = (ppm_dest_ptr) dinfo;
  register char *bufferptr;
  register JSAMPROW ptr;
#if BITS_IN_JSAMPLE != 8 || (!defined(HAVE_UNSIGNED_CHAR) && !defined(__CHAR_UNSIGNED__))
  register JDIMENSION col;
#endif

  ptr = dest->pub.buffer[0];
  bufferptr = dest->iobuffer;
#if BITS_IN_JSAMPLE == 8 && (defined(HAVE_UNSIGNED_CHAR) || defined(__CHAR_UNSIGNED__))
  MEMCOPY(bufferptr, ptr, dest->samples_per_row);
#else
  for (col = dest->samples_per_row; col > 0; col--) {
    PUTPPMSAMPLE(bufferptr, GETJSAMPLE(*ptr++));
  }
#endif
  (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->buffer_width);
}