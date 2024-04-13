g_file_new_for_path (const char *path)
{
  g_return_val_if_fail (path != NULL, NULL);

  return g_vfs_get_file_for_path (g_vfs_get_default (), path);
}