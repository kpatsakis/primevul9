read_colormap (bmp_source_ptr sinfo, int cmaplen, int mapentrysize)
/* Read the colormap from a BMP file */
{
  int i;

  switch (mapentrysize) {
  case 3:
    /* BGR format (occurs in OS/2 files) */
    for (i = 0; i < cmaplen; i++) {
      sinfo->colormap[2][i] = (JSAMPLE) read_byte(sinfo);
      sinfo->colormap[1][i] = (JSAMPLE) read_byte(sinfo);
      sinfo->colormap[0][i] = (JSAMPLE) read_byte(sinfo);
    }
    break;
  case 4:
    /* BGR0 format (occurs in MS Windows files) */
    for (i = 0; i < cmaplen; i++) {
      sinfo->colormap[2][i] = (JSAMPLE) read_byte(sinfo);
      sinfo->colormap[1][i] = (JSAMPLE) read_byte(sinfo);
      sinfo->colormap[0][i] = (JSAMPLE) read_byte(sinfo);
      (void) read_byte(sinfo);
    }
    break;
  default:
    ERREXIT(sinfo->cinfo, JERR_BMP_BADCMAP);
    break;
  }
}