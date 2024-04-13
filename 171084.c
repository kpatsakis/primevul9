replace_async_data_free (ReplaceAsyncData *data)
{
  if (data->stream)
    g_object_unref (data->stream);
  g_free (data->etag);
  g_free (data);
}