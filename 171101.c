copy_stream_with_progress (GInputStream           *in,
                           GOutputStream          *out,
                           GFile                  *source,
                           GCancellable           *cancellable,
                           GFileProgressCallback   progress_callback,
                           gpointer                progress_callback_data,
                           GError                **error)
{
  gssize n_read;
  gsize n_written;
  goffset current_size;
  char *buffer;
  gboolean res;
  goffset total_size;
  GFileInfo *info;

  total_size = -1;
  /* avoid performance impact of querying total size when it's not needed */
  if (progress_callback)
    {
      info = g_file_input_stream_query_info (G_FILE_INPUT_STREAM (in),
                                             G_FILE_ATTRIBUTE_STANDARD_SIZE,
                                             cancellable, NULL);
      if (info)
        {
          if (g_file_info_has_attribute (info, G_FILE_ATTRIBUTE_STANDARD_SIZE))
            total_size = g_file_info_get_size (info);
          g_object_unref (info);
        }

      if (total_size == -1)
        {
          info = g_file_query_info (source,
                                    G_FILE_ATTRIBUTE_STANDARD_SIZE,
                                    G_FILE_QUERY_INFO_NONE,
                                    cancellable, NULL);
          if (info)
            {
              if (g_file_info_has_attribute (info, G_FILE_ATTRIBUTE_STANDARD_SIZE))
                total_size = g_file_info_get_size (info);
              g_object_unref (info);
            }
        }
    }

  if (total_size == -1)
    total_size = 0;

  buffer = g_malloc0 (STREAM_BUFFER_SIZE);
  current_size = 0;
  res = TRUE;
  while (TRUE)
    {
      n_read = g_input_stream_read (in, buffer, STREAM_BUFFER_SIZE, cancellable, error);
      if (n_read == -1)
        {
          res = FALSE;
          break;
        }

      if (n_read == 0)
        break;

      current_size += n_read;

      res = g_output_stream_write_all (out, buffer, n_read, &n_written, cancellable, error);
      if (!res)
        break;

      if (progress_callback)
        progress_callback (current_size, total_size, progress_callback_data);
    }
  g_free (buffer);

  /* Make sure we send full copied size */
  if (progress_callback)
    progress_callback (current_size, total_size, progress_callback_data);

  return res;
}