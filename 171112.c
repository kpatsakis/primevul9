set_info_data_free (SetInfoAsyncData *data)
{
  if (data->info)
    g_object_unref (data->info);
  if (data->error)
    g_error_free (data->error);
  g_free (data);
}