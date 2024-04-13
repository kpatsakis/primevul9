measure_disk_usage_invoke_progress (gpointer user_data)
{
  MeasureProgress *progress = user_data;

  (* progress->callback) (progress->reporting,
                          progress->current_size, progress->num_dirs, progress->num_files,
                          progress->user_data);

  return FALSE;
}