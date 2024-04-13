g_file_query_writable_namespaces (GFile         *file,
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

  if (iface->query_writable_namespaces == NULL)
    return g_file_attribute_info_list_new ();

  my_error = NULL;
  list = (* iface->query_writable_namespaces) (file, cancellable, &my_error);

  if (list == NULL)
    {
      g_warn_if_reached();
      list = g_file_attribute_info_list_new ();
    }

  if (my_error != NULL)
    {
      if (my_error->domain == G_IO_ERROR && my_error->code == G_IO_ERROR_NOT_SUPPORTED)
        {
          g_error_free (my_error);
        }
      else
        g_propagate_error (error, my_error);
    }

  return list;
}