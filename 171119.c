replace_contents_data_free (ReplaceContentsData *data)
{
  g_bytes_unref (data->content);
  g_free (data->etag);
  g_free (data);
}