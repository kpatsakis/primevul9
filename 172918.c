put_gray_rows (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
               JDIMENSION rows_supplied)
/* This version is for grayscale OR quantized color output */
{
  bmp_dest_ptr dest = (bmp_dest_ptr) dinfo;
  JSAMPARRAY image_ptr;
  register JSAMPROW inptr, outptr;
  int pad;

  if (dest->use_inversion_array) {
    /* Access next row in virtual array */
    image_ptr = (*cinfo->mem->access_virt_sarray)
      ((j_common_ptr) cinfo, dest->whole_image,
       dest->cur_output_row, (JDIMENSION) 1, TRUE);
    dest->cur_output_row++;
    outptr = image_ptr[0];
  } else {
    outptr = dest->iobuffer;
  }

  /* Transfer data. */
  inptr = dest->pub.buffer[0];
  MEMCOPY(outptr, inptr, cinfo->output_width);
  outptr += cinfo->output_width;

  /* Zero out the pad bytes. */
  pad = dest->pad_bytes;
  while (--pad >= 0)
    *outptr++ = 0;

  if (!dest->use_inversion_array)
    (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->row_width);
}