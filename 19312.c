initialize_module_command (void)
{
  static int initialized;
  int err;

  if (!initialized)
    {
      err = npth_mutex_init (&status_file_update_lock, NULL);
      if (!err)
        initialized = 1;
    }
}