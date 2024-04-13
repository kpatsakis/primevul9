write_colormap (j_decompress_ptr cinfo, bmp_dest_ptr dest,
                int map_colors, int map_entry_size)
{
  JSAMPARRAY colormap = cinfo->colormap;
  int num_colors = cinfo->actual_number_of_colors;
  FILE *outfile = dest->pub.output_file;
  int i;

  if (colormap != NULL) {
    if (cinfo->out_color_components == 3) {
      /* Normal case with RGB colormap */
      for (i = 0; i < num_colors; i++) {
        putc(GETJSAMPLE(colormap[2][i]), outfile);
        putc(GETJSAMPLE(colormap[1][i]), outfile);
        putc(GETJSAMPLE(colormap[0][i]), outfile);
        if (map_entry_size == 4)
          putc(0, outfile);
      }
    } else {
      /* Grayscale colormap (only happens with grayscale quantization) */
      for (i = 0; i < num_colors; i++) {
        putc(GETJSAMPLE(colormap[0][i]), outfile);
        putc(GETJSAMPLE(colormap[0][i]), outfile);
        putc(GETJSAMPLE(colormap[0][i]), outfile);
        if (map_entry_size == 4)
          putc(0, outfile);
      }
    }
  } else {
    /* If no colormap, must be grayscale data.  Generate a linear "map". */
    for (i = 0; i < 256; i++) {
      putc(i, outfile);
      putc(i, outfile);
      putc(i, outfile);
      if (map_entry_size == 4)
        putc(0, outfile);
    }
  }
  /* Pad colormap with zeros to ensure specified number of colormap entries */
  if (i > map_colors)
    ERREXIT1(cinfo, JERR_TOO_MANY_COLORS, i);
  for (; i < map_colors; i++) {
    putc(0, outfile);
    putc(0, outfile);
    putc(0, outfile);
    if (map_entry_size == 4)
      putc(0, outfile);
  }
}