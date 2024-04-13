g_file_set_attribute_byte_string  (GFile                *file,
                                   const gchar          *attribute,
                                   const gchar          *value,
                                   GFileQueryInfoFlags   flags,
                                   GCancellable         *cancellable,
                                   GError              **error)
{
  return g_file_set_attribute (file, attribute,
                               G_FILE_ATTRIBUTE_TYPE_BYTE_STRING, (gpointer)value,
                               flags, cancellable, error);
}