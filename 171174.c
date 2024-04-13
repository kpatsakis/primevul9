g_file_replace_contents (GFile             *file,
                         const char        *contents,
                         gsize              length,
                         const char        *etag,
                         gboolean           make_backup,
                         GFileCreateFlags   flags,
                         char             **new_etag,
                         GCancellable      *cancellable,
                         GError           **error)
{
  GFileOutputStream *out;
  gsize pos, remainder;
  gssize res;
  gboolean ret;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (contents != NULL, FALSE);

  out = g_file_replace (file, etag, make_backup, flags, cancellable, error);
  if (out == NULL)
    return FALSE;

  pos = 0;
  remainder = length;
  while (remainder > 0 &&
         (res = g_output_stream_write (G_OUTPUT_STREAM (out),
                                       contents + pos,
                                       MIN (remainder, GET_CONTENT_BLOCK_SIZE),
                                       cancellable,
                                       error)) > 0)
    {
      pos += res;
      remainder -= res;
    }

  if (remainder > 0 && res < 0)
    {
      /* Ignore errors on close */
      g_output_stream_close (G_OUTPUT_STREAM (out), cancellable, NULL);
      g_object_unref (out);

      /* error is set already */
      return FALSE;
    }

  ret = g_output_stream_close (G_OUTPUT_STREAM (out), cancellable, error);

  if (new_etag)
    *new_etag = g_file_output_stream_get_etag (out);

  g_object_unref (out);

  return ret;
}