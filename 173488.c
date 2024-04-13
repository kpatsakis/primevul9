psd_to_gimp_color_map (guchar *map256)
{
  guchar *tmpmap;
  gint    i;

  tmpmap = g_malloc (3 * 256);

  for (i = 0; i < 256; ++i)
    {
      tmpmap[i*3  ] = map256[i];
      tmpmap[i*3+1] = map256[i+256];
      tmpmap[i*3+2] = map256[i+512];
    }

  memcpy (map256, tmpmap, 3 * 256);
  g_free (tmpmap);
}