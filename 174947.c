ReadColorMap (FILE     *fd,
              guchar    buffer[256][3],
              gint      number,
              gint      size,
              gboolean *grey)
{
  gint   i;
  guchar rgb[4];

  *grey = (number > 2);

  for (i = 0; i < number ; i++)
    {
      if (!ReadOK (fd, rgb, size))
        {
          g_message (_("Bad colormap"));
          return FALSE;
        }

      /* Bitmap save the colors in another order! But change only once! */

      buffer[i][0] = rgb[2];
      buffer[i][1] = rgb[1];
      buffer[i][2] = rgb[0];
      *grey = ((*grey) && (rgb[0]==rgb[1]) && (rgb[1]==rgb[2]));
    }

  return TRUE;
}