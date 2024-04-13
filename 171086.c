copy_async_data_free (CopyAsyncData *data)
{
  g_object_unref (data->source);
  g_object_unref (data->destination);
  g_slice_free (CopyAsyncData, data);
}