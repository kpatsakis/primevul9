add_dconf_dir_to_keyfile (GKeyFile      *keyfile,
                          DConfClient   *client,
                          const char    *dir,
                          DConfReadFlags flags)
{
  g_auto(GStrv) keys = NULL;
  int i;

  keys = dconf_client_list (client, dir, NULL);
  for (i = 0; keys[i]; i++)
    {
      g_autofree char *k = g_strconcat (dir, keys[i], NULL);
      if (dconf_is_dir (k, NULL))
        add_dconf_dir_to_keyfile (keyfile, client, k, flags);
      else if (dconf_is_key (k, NULL))
        add_dconf_key_to_keyfile (keyfile, client, k, flags);
    }
}