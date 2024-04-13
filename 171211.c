g_file_peek_path (GFile *file)
{
  if (G_IS_LOCAL_FILE (file))
    return _g_local_file_get_filename ((GLocalFile *) file);
  return file_peek_path_generic (file);
}