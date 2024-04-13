g_file_equal (GFile *file1,
              GFile *file2)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file1), FALSE);
  g_return_val_if_fail (G_IS_FILE (file2), FALSE);

  if (file1 == file2)
    return TRUE;

  if (G_TYPE_FROM_INSTANCE (file1) != G_TYPE_FROM_INSTANCE (file2))
    return FALSE;

  iface = G_FILE_GET_IFACE (file1);

  return (* iface->equal) (file1, file2);
}