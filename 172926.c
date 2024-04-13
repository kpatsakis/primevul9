put_pixel_rows (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo,
                JDIMENSION rows_supplied)
/* This version is for writing 24-bit pixels */
{
  bmp_dest_ptr dest = (bmp_dest_ptr) dinfo;
  JSAMPARRAY image_ptr;
  register JSAMPROW inptr, outptr;
  register JDIMENSION col;
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

  /* Transfer data.  Note destination values must be in BGR order
   * (even though Microsoft's own documents say the opposite).
   */
  inptr = dest->pub.buffer[0];

  if (cinfo->out_color_space == JCS_EXT_BGR) {
    MEMCOPY(outptr, inptr, dest->row_width);
    outptr += cinfo->output_width * 3;
  } else if (cinfo->out_color_space == JCS_RGB565) {
    boolean big_endian = is_big_endian();
    unsigned short *inptr2 = (unsigned short *)inptr;
    for (col = cinfo->output_width; col > 0; col--) {
      if (big_endian) {
        outptr[0] = (*inptr2 >> 5) & 0xF8;
        outptr[1] = ((*inptr2 << 5) & 0xE0) | ((*inptr2 >> 11) & 0x1C);
        outptr[2] = *inptr2 & 0xF8;
      } else {
        outptr[0] = (*inptr2 << 3) & 0xF8;
        outptr[1] = (*inptr2 >> 3) & 0xFC;
        outptr[2] = (*inptr2 >> 8) & 0xF8;
      }
      outptr += 3;
      inptr2++;
    }
  } else if (cinfo->out_color_space == JCS_CMYK) {
    for (col = cinfo->output_width; col > 0; col--) {
      /* can omit GETJSAMPLE() safely */
      JSAMPLE c = *inptr++, m = *inptr++, y = *inptr++, k = *inptr++;
      cmyk_to_rgb(c, m, y, k, outptr + 2, outptr + 1, outptr);
      outptr += 3;
    }
  } else {
    register int rindex = rgb_red[cinfo->out_color_space];
    register int gindex = rgb_green[cinfo->out_color_space];
    register int bindex = rgb_blue[cinfo->out_color_space];
    register int ps = rgb_pixelsize[cinfo->out_color_space];

    for (col = cinfo->output_width; col > 0; col--) {
      /* can omit GETJSAMPLE() safely */
      outptr[0] = inptr[bindex];
      outptr[1] = inptr[gindex];
      outptr[2] = inptr[rindex];
      outptr += 3;  inptr += ps;
    }
  }

  /* Zero out the pad bytes. */
  pad = dest->pad_bytes;
  while (--pad >= 0)
    *outptr++ = 0;

  if (!dest->use_inversion_array)
    (void) JFWRITE(dest->pub.output_file, dest->iobuffer, dest->row_width);
}