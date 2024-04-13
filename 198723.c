flatpak_run_add_pulseaudio_args (FlatpakBwrap *bwrap)
{
  g_autofree char *pulseaudio_server = flatpak_run_get_pulseaudio_server ();
  g_autofree char *pulseaudio_socket = NULL;
  g_autofree char *user_runtime_dir = flatpak_get_real_xdg_runtime_dir ();

  if (pulseaudio_server)
    pulseaudio_socket = flatpak_run_parse_pulse_server (pulseaudio_server);

  if (!pulseaudio_socket)
    pulseaudio_socket = g_build_filename (user_runtime_dir, "pulse/native", NULL);

  flatpak_bwrap_unset_env (bwrap, "PULSE_SERVER");

  if (g_file_test (pulseaudio_socket, G_FILE_TEST_EXISTS))
    {
      gboolean share_shm = FALSE; /* TODO: When do we add this? */
      g_autofree char *client_config = g_strdup_printf ("enable-shm=%s\n", share_shm ? "yes" : "no");
      g_autofree char *sandbox_socket_path = g_strdup_printf ("/run/user/%d/pulse/native", getuid ());
      g_autofree char *pulse_server = g_strdup_printf ("unix:/run/user/%d/pulse/native", getuid ());
      g_autofree char *config_path = g_strdup_printf ("/run/user/%d/pulse/config", getuid ());

      /* FIXME - error handling */
      if (!flatpak_bwrap_add_args_data (bwrap, "pulseaudio", client_config, -1, config_path, NULL))
        return;

      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", pulseaudio_socket, sandbox_socket_path,
                              NULL);

      flatpak_bwrap_set_env (bwrap, "PULSE_SERVER", pulse_server, TRUE);
      flatpak_bwrap_set_env (bwrap, "PULSE_CLIENTCONFIG", config_path, TRUE);
    }
  else
    g_debug ("Could not find pulseaudio socket");
}