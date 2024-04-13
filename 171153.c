g_file_new_for_commandline_arg (const char *arg)
{
  g_return_val_if_fail (arg != NULL, NULL);

  return new_for_cmdline_arg (arg, NULL);
}