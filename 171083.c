g_file_real_set_attributes_finish (GFile         *file,
                                   GAsyncResult  *res,
                                   GFileInfo    **info,
                                   GError       **error)
{
  SetInfoAsyncData *data;

  g_return_val_if_fail (g_task_is_valid (res, file), FALSE);

  data = g_task_get_task_data (G_TASK (res));

  if (info)
    *info = g_object_ref (data->info);

  if (error != NULL && data->error)
    *error = g_error_copy (data->error);

  return data->res;
}