g_file_query_settable_attributes (GFile         *file,
                                  GCancellable  *cancellable,
                                  GError       **error)
{
  GFileIface *iface;
  GError *my_error;
  GFileAttributeInfoList *list;

  g_return_val_if_fail (G_IS_FILE (file), NULL);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return NULL;

  iface = G_FILE_GET_IFACE (file);

  if (iface->query_settable_attributes == NULL)
    return g_file_attribute_info_list_new ();

  my_error = NULL;
  list = (* iface->query_settable_attributes) (file, cancellable, &my_error);

  if (list == NULL)
    {
      if (my_error->domain == G_IO_ERROR && my_error->code == G_IO_ERROR_NOT_SUPPORTED)
        {
          list = g_file_attribute_info_list_new ();
          g_error_free (my_error);
        }
      else
        g_propagate_error (error, my_error);
    }

  return list;
}