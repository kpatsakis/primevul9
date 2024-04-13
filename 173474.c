read_color_mode_block (PSDimage  *img_a,
                       FILE      *f,
                       GError   **error)
{
  static guchar cmap[] = {0, 0, 0, 255, 255, 255};
  guint32       block_len;

  img_a->color_map_entries = 0;
  img_a->color_map_len = 0;
  if (fread (&block_len, 4, 1, f) < 1)
    {
      psd_set_error (feof (f), errno, error);
      return -1;
    }
  block_len = GUINT32_FROM_BE (block_len);

  IFDBG(1) g_debug ("Color map block size = %d", block_len);

  if (block_len == 0)
    {
      if (img_a->color_mode == PSD_INDEXED ||
          img_a->color_mode == PSD_DUOTONE )
        {
          IFDBG(1) g_debug ("No color block for indexed or duotone image");
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                      _("The file is corrupt!"));
          return -1;
        }
    }
  else if (img_a->color_mode == PSD_INDEXED)
    {
      if (block_len != 768)
        {
          IFDBG(1) g_debug ("Invalid color block size for indexed image");
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                      _("The file is corrupt!"));
          return -1;
        }
      else
        {
          img_a->color_map_len = block_len;
          img_a->color_map = g_malloc (img_a->color_map_len);
          if (fread (img_a->color_map, block_len, 1, f) < 1)
            {
              psd_set_error (feof (f), errno, error);
              return -1;
            }
          else
            {
              psd_to_gimp_color_map (img_a->color_map);
              img_a->color_map_entries = 256;
            }
        }
    }
  else if (img_a->color_mode == PSD_DUOTONE)
    {
      img_a->color_map_len = block_len;
      img_a->color_map = g_malloc (img_a->color_map_len);
      if (fread (img_a->color_map, block_len, 1, f) < 1)
        {
          psd_set_error (feof (f), errno, error);
          return -1;
        }
    }

  /* Create color map for bitmap image */
  if (img_a->color_mode == PSD_BITMAP)
    {
      img_a->color_map_len = 6;
      img_a->color_map = g_malloc (img_a->color_map_len);
      memcpy (img_a->color_map, cmap, img_a->color_map_len);
      img_a->color_map_entries = 2;
    }
  IFDBG(2) g_debug ("Colour map data length %d", img_a->color_map_len);

  return 0;
}