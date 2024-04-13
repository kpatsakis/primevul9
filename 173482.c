get_psd_color_mode_name (PSDColorMode mode)
{
  static gchar * const psd_color_mode_names[] =
  {
    "BITMAP",
    "GRAYSCALE",
    "INDEXED",
    "RGB",
    "CMYK",
    "UNKNOWN (5)",
    "UNKNOWN (6)",
    "MULTICHANNEL",
    "DUOTONE",
    "LAB"
  };

  static gchar *err_name = NULL;

  if (mode >= PSD_BITMAP && mode <= PSD_LAB)
    return psd_color_mode_names[mode];

  g_free (err_name);
  err_name = g_strdup_printf ("UNKNOWN (%d)", mode);

  return err_name;
}