flatpak_run_get_pulse_home (void)
{
  /* Legacy path ~/.pulse is tried first, for compatibility */
  {
    const char *parent = g_get_home_dir ();
    g_autofree char *ret = g_build_filename (parent, ".pulse", NULL);

    if (g_file_test (ret, G_FILE_TEST_IS_DIR))
      return g_steal_pointer (&ret);
  }

  /* The more modern path, usually ~/.config/pulse */
  {
    const char *parent = g_get_user_config_dir ();
    /* Usually ~/.config/pulse */
    g_autofree char *ret = g_build_filename (parent, "pulse", NULL);

    if (g_file_test (ret, G_FILE_TEST_IS_DIR))
      return g_steal_pointer (&ret);
  }

  return NULL;
}