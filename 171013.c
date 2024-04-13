set_info_async_thread (GTask        *task,
                       gpointer      object,
                       gpointer      task_data,
                       GCancellable *cancellable)
{
  SetInfoAsyncData *data = task_data;

  data->error = NULL;
  data->res = g_file_set_attributes_from_info (G_FILE (object),
                                               data->info,
                                               data->flags,
                                               cancellable,
                                               &data->error);
}