g_file_has_prefix (GFile *file,
                   GFile *prefix)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (G_IS_FILE (prefix), FALSE);

  if (G_TYPE_FROM_INSTANCE (file) != G_TYPE_FROM_INSTANCE (prefix))
    return FALSE;

  iface = G_FILE_GET_IFACE (file);

  /* The vtable function differs in arg order since
   * we're using the old contains_file call
   */
  return (* iface->prefix_matches) (prefix, file);
}