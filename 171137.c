g_file_new_tmp (const char     *tmpl,
                GFileIOStream **iostream,
                GError        **error)
{
  gint fd;
  gchar *path;
  GFile *file;
  GFileOutputStream *output;

  g_return_val_if_fail (iostream != NULL, NULL);

  fd = g_file_open_tmp (tmpl, &path, error);
  if (fd == -1)
    return NULL;

  file = g_file_new_for_path (path);

  output = _g_local_file_output_stream_new (fd);
  *iostream = _g_local_file_io_stream_new (G_LOCAL_FILE_OUTPUT_STREAM (output));

  g_object_unref (output);
  g_free (path);

  return file;
}