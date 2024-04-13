get_32bit_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading 32-bit pixels */
{
  bmp_source_ptr source = (bmp_source_ptr) sinfo;
  JSAMPARRAY image_ptr;
  register JSAMPROW inptr, outptr;
  register JDIMENSION col;

  if (source->use_inversion_array) {
    /* Fetch next row from virtual array */
    source->source_row--;
    image_ptr = (*cinfo->mem->access_virt_sarray)
      ((j_common_ptr) cinfo, source->whole_image,
       source->source_row, (JDIMENSION) 1, FALSE);
    inptr = image_ptr[0];
  } else {
    if (! ReadOK(source->pub.input_file, source->iobuffer, source->row_width))
      ERREXIT(cinfo, JERR_INPUT_EOF);
    inptr = source->iobuffer;
  }

  /* Transfer data.  Note source values are in BGR order
   * (even though Microsoft's own documents say the opposite).
   */
  outptr = source->pub.buffer[0];
  if (cinfo->in_color_space == JCS_EXT_BGRX ||
      cinfo->in_color_space == JCS_EXT_BGRA) {
    MEMCOPY(outptr, inptr, source->row_width);
  } else if (cinfo->in_color_space == JCS_GRAYSCALE) {
    for (col = cinfo->image_width; col > 0; col--) {
      /* can omit GETJSAMPLE() safely */
      JSAMPLE b = *inptr++, g = *inptr++, r = *inptr++;
      *outptr++ = RGB2GRAY(r, g, b);
    }
  } else if (cinfo->in_color_space == JCS_CMYK) {
    for (col = cinfo->image_width; col > 0; col--) {
      /* can omit GETJSAMPLE() safely */
      JSAMPLE b = *inptr++, g = *inptr++, r = *inptr++;
      rgb_to_cmyk(r, g, b, outptr, outptr + 1, outptr + 2, outptr + 3);
      inptr++;                          /* skip the 4th byte (Alpha channel) */
      outptr += 4;
    }
  } else {
    register int rindex = rgb_red[cinfo->in_color_space];
    register int gindex = rgb_green[cinfo->in_color_space];
    register int bindex = rgb_blue[cinfo->in_color_space];
    register int aindex = alpha_index[cinfo->in_color_space];
    register int ps = rgb_pixelsize[cinfo->in_color_space];

    if (aindex >= 0) {
      for (col = cinfo->image_width; col > 0; col--) {
        outptr[bindex] = *inptr++;      /* can omit GETJSAMPLE() safely */
        outptr[gindex] = *inptr++;
        outptr[rindex] = *inptr++;
        outptr[aindex] = *inptr++;
        outptr += ps;
      }
    } else {
      for (col = cinfo->image_width; col > 0; col--) {
        outptr[bindex] = *inptr++;      /* can omit GETJSAMPLE() safely */
        outptr[gindex] = *inptr++;
        outptr[rindex] = *inptr++;
        inptr++;                        /* skip the 4th byte (Alpha channel) */
        outptr += ps;
      }
    }
  }

  return 1;
}