g_file_set_attribute_int32 (GFile                *file,
                            const gchar          *attribute,
                            gint32                value,
                            GFileQueryInfoFlags   flags,
                            GCancellable         *cancellable,
                            GError              **error)
{
  return g_file_set_attribute (file, attribute,
                               G_FILE_ATTRIBUTE_TYPE_INT32, &value,
                               flags, cancellable, error);
}