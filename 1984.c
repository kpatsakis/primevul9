add_dconf_locks_to_list (GString     *s,
                         DConfClient *client,
                         const char  *dir)
{
  g_auto(GStrv) locks = NULL;
  int i;

  locks = dconf_client_list_locks (client, dir, NULL);
  for (i = 0; locks[i]; i++)
    {
      g_string_append (s, locks[i]);
      g_string_append_c (s, '\n');
    }
}