g_file_new_for_commandline_arg_and_cwd (const gchar *arg,
                                        const gchar *cwd)
{
  g_return_val_if_fail (arg != NULL, NULL);
  g_return_val_if_fail (cwd != NULL, NULL);

  return new_for_cmdline_arg (arg, cwd);
}