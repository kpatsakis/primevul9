new_for_cmdline_arg (const gchar *arg,
                     const gchar *cwd)
{
  GFile *file;
  char *filename;

  if (g_path_is_absolute (arg))
    return g_file_new_for_path (arg);

  if (has_valid_scheme (arg))
    return g_file_new_for_uri (arg);

  if (cwd == NULL)
    {
      char *current_dir;

      current_dir = g_get_current_dir ();
      filename = g_build_filename (current_dir, arg, NULL);
      g_free (current_dir);
    }
  else
    filename = g_build_filename (cwd, arg, NULL);

  file = g_file_new_for_path (filename);
  g_free (filename);

  return file;
}