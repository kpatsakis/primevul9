g_file_set_attribute_string (GFile                *file,
                             const char           *attribute,
                             const char           *value,
                             GFileQueryInfoFlags   flags,
                             GCancellable         *cancellable,
                             GError              **error)
{
  return g_file_set_attribute (file, attribute,
                               G_FILE_ATTRIBUTE_TYPE_STRING, (gpointer)value,
                               flags, cancellable, error);
}