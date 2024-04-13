flatpak_run_add_pulseaudio_args (FlatpakBwrap *bwrap)
{
  g_autofree char *pulseaudio_server = flatpak_run_get_pulseaudio_server ();
  g_autofree char *pulseaudio_socket = NULL;
  g_autofree char *pulse_runtime_dir = flatpak_run_get_pulse_runtime_dir ();

  if (pulseaudio_server)
    pulseaudio_socket = flatpak_run_parse_pulse_server (pulseaudio_server);

  if (!pulseaudio_socket)
    {
      pulseaudio_socket = g_build_filename (pulse_runtime_dir, "native", NULL);

      if (!g_file_test (pulseaudio_socket, G_FILE_TEST_EXISTS))
        g_clear_pointer (&pulseaudio_socket, g_free);
    }

  if (!pulseaudio_socket)
    {
      pulseaudio_socket = realpath ("/var/run/pulse/native", NULL);

      if (pulseaudio_socket && !g_file_test (pulseaudio_socket, G_FILE_TEST_EXISTS))
        g_clear_pointer (&pulseaudio_socket, g_free);
    }

  flatpak_bwrap_unset_env (bwrap, "PULSE_SERVER");

  if (pulseaudio_socket && g_file_test (pulseaudio_socket, G_FILE_TEST_EXISTS))
    {
      static const char sandbox_socket_path[] = "/run/flatpak/pulse/native";
      static const char pulse_server[] = "unix:/run/flatpak/pulse/native";
      static const char config_path[] = "/run/flatpak/pulse/config";
      gboolean share_shm = FALSE; /* TODO: When do we add this? */
      g_autofree char *client_config = g_strdup_printf ("enable-shm=%s\n", share_shm ? "yes" : "no");

      /* FIXME - error handling */
      if (!flatpak_bwrap_add_args_data (bwrap, "pulseaudio", client_config, -1, config_path, NULL))
        return;

      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", pulseaudio_socket, sandbox_socket_path,
                              NULL);

      flatpak_bwrap_set_env (bwrap, "PULSE_SERVER", pulse_server, TRUE);
      flatpak_bwrap_set_env (bwrap, "PULSE_CLIENTCONFIG", config_path, TRUE);
      flatpak_bwrap_add_runtime_dir_member (bwrap, "pulse");
    }
  else
    g_debug ("Could not find pulseaudio socket");

  /* Also allow ALSA access. This was added in 1.8, and is not ideally named. However,
   * since the practical permission of ALSA and PulseAudio are essentially the same, and
   * since we don't want to add more permissions for something we plan to replace with
   * portals/pipewire going forward we reinterpret pulseaudio to also mean ALSA.
   */
  if (g_file_test ("/dev/snd", G_FILE_TEST_IS_DIR))
    flatpak_bwrap_add_args (bwrap, "--dev-bind", "/dev/snd", "/dev/snd", NULL);
}