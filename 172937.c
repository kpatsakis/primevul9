write_os2_header (j_decompress_ptr cinfo, bmp_dest_ptr dest)
/* Write an OS2-style BMP file header, including colormap if needed */
{
  char bmpfileheader[14];
  char bmpcoreheader[12];
  long headersize, bfSize;
  int bits_per_pixel, cmap_entries;

  /* Compute colormap size and total file size */
  if (cinfo->out_color_space == JCS_RGB ||
      (cinfo->out_color_space >= JCS_EXT_RGB &&
       cinfo->out_color_space <= JCS_EXT_ARGB)) {
    if (cinfo->quantize_colors) {
      /* Colormapped RGB */
      bits_per_pixel = 8;
      cmap_entries = 256;
    } else {
      /* Unquantized, full color RGB */
      bits_per_pixel = 24;
      cmap_entries = 0;
    }
  } else if (cinfo->out_color_space == JCS_RGB565 ||
             cinfo->out_color_space == JCS_CMYK) {
    bits_per_pixel = 24;
    cmap_entries   = 0;
  } else {
    /* Grayscale output.  We need to fake a 256-entry colormap. */
    bits_per_pixel = 8;
    cmap_entries = 256;
  }
  /* File size */
  headersize = 14 + 12 + cmap_entries * 3; /* Header and colormap */
  bfSize = headersize + (long) dest->row_width * (long) cinfo->output_height;

  /* Set unused fields of header to 0 */
  MEMZERO(bmpfileheader, sizeof(bmpfileheader));
  MEMZERO(bmpcoreheader, sizeof(bmpcoreheader));

  /* Fill the file header */
  bmpfileheader[0] = 0x42;      /* first 2 bytes are ASCII 'B', 'M' */
  bmpfileheader[1] = 0x4D;
  PUT_4B(bmpfileheader, 2, bfSize); /* bfSize */
  /* we leave bfReserved1 & bfReserved2 = 0 */
  PUT_4B(bmpfileheader, 10, headersize); /* bfOffBits */

  /* Fill the info header (Microsoft calls this a BITMAPCOREHEADER) */
  PUT_2B(bmpcoreheader, 0, 12); /* bcSize */
  PUT_2B(bmpcoreheader, 4, cinfo->output_width); /* bcWidth */
  PUT_2B(bmpcoreheader, 6, cinfo->output_height); /* bcHeight */
  PUT_2B(bmpcoreheader, 8, 1);  /* bcPlanes - must be 1 */
  PUT_2B(bmpcoreheader, 10, bits_per_pixel); /* bcBitCount */

  if (JFWRITE(dest->pub.output_file, bmpfileheader, 14) != (size_t) 14)
    ERREXIT(cinfo, JERR_FILE_WRITE);
  if (JFWRITE(dest->pub.output_file, bmpcoreheader, 12) != (size_t) 12)
    ERREXIT(cinfo, JERR_FILE_WRITE);

  if (cmap_entries > 0)
    write_colormap(cinfo, dest, cmap_entries, 3);
}