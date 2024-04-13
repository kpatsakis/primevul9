g_file_get_child_for_display_name (GFile      *file,
                                   const char *display_name,
                                   GError **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (display_name != NULL, NULL);

  iface = G_FILE_GET_IFACE (file);

  return (* iface->get_child_for_display_name) (file, display_name, error);
}