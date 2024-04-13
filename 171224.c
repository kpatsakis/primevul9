copy_async_progress_in_main (gpointer user_data)
{
  ProgressData *progress = user_data;
  CopyAsyncData *data = progress->data;

  data->progress_cb (progress->current_num_bytes,
                     progress->total_num_bytes,
                     data->progress_cb_data);

  return FALSE;
}