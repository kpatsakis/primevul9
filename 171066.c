g_file_new_for_uri (const char *uri)
{
  g_return_val_if_fail (uri != NULL, NULL);

  return g_vfs_get_file_for_uri (g_vfs_get_default (), uri);
}