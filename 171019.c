g_file_set_attribute_uint32 (GFile                *file,
                             const gchar          *attribute,
                             guint32               value,
                             GFileQueryInfoFlags   flags,
                             GCancellable         *cancellable,
                             GError              **error)
{
  return g_file_set_attribute (file, attribute,
                               G_FILE_ATTRIBUTE_TYPE_UINT32, &value,
                               flags, cancellable, error);
}