static gboolean ReadColorMap(FILE * fd, unsigned char buffer[256][3], int number, int size, 
	gboolean *Grey,
	at_exception_type *exp)
{
  int i;
  unsigned char rgb[4];

  *Grey = (number > 2);
  for (i = 0; i < number; i++) {
    if (!ReadOK(fd, rgb, size)) {
      LOG ("Bad colormap\n");
      at_exception_fatal (exp, "Bad colormap");
      return FALSE;
    }

    /* Bitmap save the colors in another order! But change only once! */

    buffer[i][0] = rgb[2];
    buffer[i][1] = rgb[1];
    buffer[i][2] = rgb[0];
    *Grey = ((*Grey) && (rgb[0] == rgb[1]) && (rgb[1] == rgb[2]));
  }
cleanup:
  return TRUE;
}