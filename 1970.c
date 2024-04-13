flatpak_run_get_pulseaudio_server (void)
{
  const char * pulse_clientconfig;
  char *pulse_server;
  g_autofree char *pulse_user_config = NULL;

  pulse_server = g_strdup (g_getenv ("PULSE_SERVER"));
  if (pulse_server)
    return pulse_server;

  pulse_clientconfig = g_getenv ("PULSE_CLIENTCONFIG");
  if (pulse_clientconfig)
    return flatpak_run_get_pulseaudio_server_user_config (pulse_clientconfig);

  pulse_user_config = g_build_filename (g_get_user_config_dir (), "pulse/client.conf", NULL);
  pulse_server = flatpak_run_get_pulseaudio_server_user_config (pulse_user_config);
  if (pulse_server)
    return pulse_server;

  pulse_server = flatpak_run_get_pulseaudio_server_user_config ("/etc/pulse/client.conf");
  if (pulse_server)
    return pulse_server;

  return NULL;
}