g_file_parse_name (const char *parse_name)
{
  g_return_val_if_fail (parse_name != NULL, NULL);

  return g_vfs_parse_name (g_vfs_get_default (), parse_name);
}