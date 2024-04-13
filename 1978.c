flatpak_run_get_pulse_runtime_dir (void)
{
  const char *val = NULL;

  val = g_getenv ("PULSE_RUNTIME_PATH");

  if (val != NULL)
    return realpath (val, NULL);

  {
    const char *user_runtime_dir = g_get_user_runtime_dir ();

    if (user_runtime_dir != NULL)
      {
        g_autofree char *dir = g_build_filename (user_runtime_dir, "pulse", NULL);

        if (g_file_test (dir, G_FILE_TEST_IS_DIR))
          return realpath (dir, NULL);
      }
  }

  {
    g_autofree char *pulse_home = flatpak_run_get_pulse_home ();
    g_autofree char *machine_id = flatpak_run_get_pulse_machine_id ();

    if (pulse_home != NULL && machine_id != NULL)
      {
        /* This is usually a symlink, but we take its realpath() anyway */
        g_autofree char *dir = g_strdup_printf ("%s/%s-runtime", pulse_home, machine_id);

        if (g_file_test (dir, G_FILE_TEST_IS_DIR))
          return realpath (dir, NULL);
      }
  }

  return NULL;
}