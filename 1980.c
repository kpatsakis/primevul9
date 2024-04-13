flatpak_run_parse_pulse_server (const char *value)
{
  g_auto(GStrv) servers = g_strsplit (value, " ", 0);
  gsize i;

  for (i = 0; servers[i] != NULL; i++)
    {
      const char *server = servers[i];
      if (g_str_has_prefix (server, "{"))
        {
          const char * closing = strstr (server, "}");
          if (closing == NULL)
            continue;
          server = closing + 1;
        }
      if (g_str_has_prefix (server, "unix:"))
        return g_strdup (server + 5);
    }

  return NULL;
}