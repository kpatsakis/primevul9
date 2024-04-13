query_info_data_free (QueryInfoAsyncData *data)
{
  g_free (data->attributes);
  g_free (data);
}