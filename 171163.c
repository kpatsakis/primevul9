g_file_set_attribute_int64 (GFile                *file,
                            const gchar          *attribute,
                            gint64                value,
                            GFileQueryInfoFlags   flags,
                            GCancellable         *cancellable,
                            GError              **error)
{
  return g_file_set_attribute (file, attribute,
                               G_FILE_ATTRIBUTE_TYPE_INT64, &value,
                               flags, cancellable, error);
}