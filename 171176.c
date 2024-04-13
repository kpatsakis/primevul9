g_file_new_build_filename (const gchar *first_element,
                           ...)
{
  gchar *str;
  GFile *file;
  va_list args;

  g_return_val_if_fail (first_element != NULL, NULL);

  va_start (args, first_element);
  str = g_build_filename_valist (first_element, &args);
  va_end (args);

  file = g_file_new_for_path (str);
  g_free (str);

  return file;
}