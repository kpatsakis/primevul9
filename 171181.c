load_contents_data_free (LoadContentsData *data)
{
  if (data->content)
    g_byte_array_free (data->content, TRUE);
  g_free (data->etag);
  g_free (data);
}