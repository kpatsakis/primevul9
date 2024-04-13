write_bmp_header (j_decompress_ptr cinfo, bmp_dest_ptr dest)
/* Write a Windows-style BMP file header, including colormap if needed */
{
  char bmpfileheader[14];
  char bmpinfoheader[40];
#define PUT_2B(array,offset,value)  \
        (array[offset] = (char) ((value) & 0xFF), \
         array[offset+1] = (char) (((value) >> 8) & 0xFF))
#define PUT_4B(array,offset,value)  \
        (array[offset] = (char) ((value) & 0xFF), \
         array[offset+1] = (char) (((value) >> 8) & 0xFF), \
         array[offset+2] = (char) (((value) >> 16) & 0xFF), \
         array[offset+3] = (char) (((value) >> 24) & 0xFF))
  long headersize, bfSize;
  int bits_per_pixel, cmap_entries;

  /* Compute colormap size and total file size */
  if (IsExtRGB(cinfo->out_color_space)) {
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
  headersize = 14 + 40 + cmap_entries * 4; /* Header and colormap */
  bfSize = headersize + (long) dest->row_width * (long) cinfo->output_height;

  /* Set unused fields of header to 0 */
  MEMZERO(bmpfileheader, sizeof(bmpfileheader));
  MEMZERO(bmpinfoheader, sizeof(bmpinfoheader));

  /* Fill the file header */
  bmpfileheader[0] = 0x42;      /* first 2 bytes are ASCII 'B', 'M' */
  bmpfileheader[1] = 0x4D;
  PUT_4B(bmpfileheader, 2, bfSize); /* bfSize */
  /* we leave bfReserved1 & bfReserved2 = 0 */
  PUT_4B(bmpfileheader, 10, headersize); /* bfOffBits */

  /* Fill the info header (Microsoft calls this a BITMAPINFOHEADER) */
  PUT_2B(bmpinfoheader, 0, 40); /* biSize */
  PUT_4B(bmpinfoheader, 4, cinfo->output_width); /* biWidth */
  PUT_4B(bmpinfoheader, 8, cinfo->output_height); /* biHeight */
  PUT_2B(bmpinfoheader, 12, 1); /* biPlanes - must be 1 */
  PUT_2B(bmpinfoheader, 14, bits_per_pixel); /* biBitCount */
  /* we leave biCompression = 0, for none */
  /* we leave biSizeImage = 0; this is correct for uncompressed data */
  if (cinfo->density_unit == 2) { /* if have density in dots/cm, then */
    PUT_4B(bmpinfoheader, 24, (long) (cinfo->X_density*100)); /* XPels/M */
    PUT_4B(bmpinfoheader, 28, (long) (cinfo->Y_density*100)); /* XPels/M */
  }
  PUT_2B(bmpinfoheader, 32, cmap_entries); /* biClrUsed */
  /* we leave biClrImportant = 0 */

  if (JFWRITE(dest->pub.output_file, bmpfileheader, 14) != (size_t) 14)
    ERREXIT(cinfo, JERR_FILE_WRITE);
  if (JFWRITE(dest->pub.output_file, bmpinfoheader, 40) != (size_t) 40)
    ERREXIT(cinfo, JERR_FILE_WRITE);

  if (cmap_entries > 0)
    write_colormap(cinfo, dest, cmap_entries, 4);
}