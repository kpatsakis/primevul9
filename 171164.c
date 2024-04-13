g_file_set_attribute_uint64 (GFile                *file,
                             const gchar          *attribute,
                             guint64               value,
                             GFileQueryInfoFlags   flags,
                             GCancellable         *cancellable,
                             GError              **error)
 {
  return g_file_set_attribute (file, attribute,
                               G_FILE_ATTRIBUTE_TYPE_UINT64, &value,
                               flags, cancellable, error);
}