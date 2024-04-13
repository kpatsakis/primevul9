g_file_load_contents (GFile         *file,
                      GCancellable  *cancellable,
                      char         **contents,
                      gsize         *length,
                      char         **etag_out,
                      GError       **error)
{
  GFileInputStream *in;
  GByteArray *content;
  gsize pos;
  gssize res;
  GFileInfo *info;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (contents != NULL, FALSE);

  in = g_file_read (file, cancellable, error);
  if (in == NULL)
    return FALSE;

  content = g_byte_array_new ();
  pos = 0;

  g_byte_array_set_size (content, pos + GET_CONTENT_BLOCK_SIZE + 1);
  while ((res = g_input_stream_read (G_INPUT_STREAM (in),
                                     content->data + pos,
                                     GET_CONTENT_BLOCK_SIZE,
                                     cancellable, error)) > 0)
    {
      pos += res;
      g_byte_array_set_size (content, pos + GET_CONTENT_BLOCK_SIZE + 1);
    }

  if (etag_out)
    {
      *etag_out = NULL;

      info = g_file_input_stream_query_info (in,
                                             G_FILE_ATTRIBUTE_ETAG_VALUE,
                                             cancellable,
                                             NULL);
      if (info)
        {
          *etag_out = g_strdup (g_file_info_get_etag (info));
          g_object_unref (info);
        }
    }

  /* Ignore errors on close */
  g_input_stream_close (G_INPUT_STREAM (in), cancellable, NULL);
  g_object_unref (in);

  if (res < 0)
    {
      /* error is set already */
      g_byte_array_free (content, TRUE);
      return FALSE;
    }

  if (length)
    *length = pos;

  /* Zero terminate (we got an extra byte allocated for this */
  content->data[pos] = 0;

  *contents = (char *)g_byte_array_free (content, FALSE);

  return TRUE;
}