g_file_set_attributes_finish (GFile         *file,
                              GAsyncResult  *result,
                              GFileInfo    **info,
                              GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  /* No standard handling of errors here, as we must set info even
   * on errors
   */
  iface = G_FILE_GET_IFACE (file);
  return (* iface->set_attributes_finish) (file, result, info, error);
}