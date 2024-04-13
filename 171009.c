g_file_get_relative_path (GFile *parent,
                          GFile *descendant)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (parent), NULL);
  g_return_val_if_fail (G_IS_FILE (descendant), NULL);

  if (G_TYPE_FROM_INSTANCE (parent) != G_TYPE_FROM_INSTANCE (descendant))
    return NULL;

  iface = G_FILE_GET_IFACE (parent);

  return (* iface->get_relative_path) (parent, descendant);
}