get_dconf_data (const char  *app_id,
                const char **paths,
                const char  *migrate_path,
                char       **defaults,
                gsize       *defaults_size,
                char       **values,
                gsize       *values_size,
                char       **locks,
                gsize       *locks_size)
{
#ifdef HAVE_DCONF
  DConfClient *client = NULL;
  g_autofree char *prefix = NULL;
#endif
  g_autoptr(GKeyFile) defaults_data = NULL;
  g_autoptr(GKeyFile) values_data = NULL;
  g_autoptr(GString) locks_data = NULL;

  defaults_data = g_key_file_new ();
  values_data = g_key_file_new ();
  locks_data = g_string_new ("");

#ifdef HAVE_DCONF

  client = dconf_client_new ();

  prefix = dconf_path_for_app_id (app_id);

  if (migrate_path)
    {
      g_debug ("Add values in dir %s", migrate_path);
      if (path_is_similar (migrate_path, prefix))
        add_dconf_dir_to_keyfile (values_data, client, migrate_path, DCONF_READ_USER_VALUE);
      else
        g_warning ("Ignoring D-Conf migrate-path setting %s", migrate_path);
    }

  g_debug ("Add defaults in dir %s", prefix);
  add_dconf_dir_to_keyfile (defaults_data, client, prefix, DCONF_READ_DEFAULT_VALUE);

  g_debug ("Add locks in dir %s", prefix);
  add_dconf_locks_to_list (locks_data, client, prefix);

  /* We allow extra paths for defaults and locks, but not for user values */
  if (paths)
    {
      int i;
      for (i = 0; paths[i]; i++)
        {
          if (dconf_is_dir (paths[i], NULL))
            {
              g_debug ("Add defaults in dir %s", paths[i]);
              add_dconf_dir_to_keyfile (defaults_data, client, paths[i], DCONF_READ_DEFAULT_VALUE);

              g_debug ("Add locks in dir %s", paths[i]);
              add_dconf_locks_to_list (locks_data, client, paths[i]);
            }
          else if (dconf_is_key (paths[i], NULL))
            {
              g_debug ("Add individual key %s", paths[i]);
              add_dconf_key_to_keyfile (defaults_data, client, paths[i], DCONF_READ_DEFAULT_VALUE);
              add_dconf_key_to_keyfile (values_data, client, paths[i], DCONF_READ_USER_VALUE);
            }
          else
            {
              g_warning ("Ignoring settings path '%s': neither dir nor key", paths[i]);
            }
        }
    }
#endif

  *defaults = g_key_file_to_data (defaults_data, defaults_size, NULL);
  *values = g_key_file_to_data (values_data, values_size, NULL);
  *locks_size = locks_data->len;
  *locks = g_string_free (g_steal_pointer (&locks_data), FALSE);

#ifdef HAVE_DCONF
  g_object_unref (client);
#endif
}