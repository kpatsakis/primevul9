flatpak_run_get_cups_server_name (void)
{
  g_autofree char * cups_server = NULL;
  g_autofree char * cups_config_path = NULL;

  /* TODO
   * we don't currently support cups servers located on the network, if such
   * server is detected, we simply ignore it and in the worst case we fallback
   * to the default socket
   */
  cups_server = g_strdup (g_getenv ("CUPS_SERVER"));
  if (cups_server && flatpak_run_cups_check_server_is_socket (cups_server))
    return g_steal_pointer (&cups_server);
  g_clear_pointer (&cups_server, g_free);

  cups_config_path = g_build_filename (g_get_home_dir (), ".cups/client.conf", NULL);
  cups_server = flatpak_run_get_cups_server_name_config (cups_config_path);
  if (cups_server && flatpak_run_cups_check_server_is_socket (cups_server))
    return g_steal_pointer (&cups_server);
  g_clear_pointer (&cups_server, g_free);

  cups_server = flatpak_run_get_cups_server_name_config ("/etc/cups/client.conf");
  if (cups_server && flatpak_run_cups_check_server_is_socket (cups_server))
    return g_steal_pointer (&cups_server);

  // Fallback to default socket
  return g_strdup ("/var/run/cups/cups.sock");
}