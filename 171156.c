replace_rw_async_data_free (ReplaceRWAsyncData *data)
{
  g_free (data->etag);
  g_free (data);
}