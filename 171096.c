g_file_get_child (GFile      *file,
                  const char *name)
{
  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  return g_file_resolve_relative_path (file, name);
}