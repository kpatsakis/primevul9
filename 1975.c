flatpak_run_cups_check_server_is_socket (const char *server)
{
  if (g_str_has_prefix (server, "/") && strstr (server, ":") == NULL)
    return TRUE;

  return FALSE;
}