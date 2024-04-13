flatpak_run_add_dconf_args (FlatpakBwrap *bwrap,
                            const char   *app_id,
                            GKeyFile     *metakey,
                            GError      **error)
{
  g_auto(GStrv) paths = NULL;
  g_autofree char *migrate_path = NULL;
  g_autofree char *defaults = NULL;
  g_autofree char *values = NULL;
  g_autofree char *locks = NULL;
  gsize defaults_size;
  gsize values_size;
  gsize locks_size;

  if (metakey)
    {
      paths = g_key_file_get_string_list (metakey,
                                          FLATPAK_METADATA_GROUP_DCONF,
                                          FLATPAK_METADATA_KEY_DCONF_PATHS,
                                          NULL, NULL);
      migrate_path = g_key_file_get_string (metakey,
                                            FLATPAK_METADATA_GROUP_DCONF,
                                            FLATPAK_METADATA_KEY_DCONF_MIGRATE_PATH,
                                            NULL);
    }

  get_dconf_data (app_id,
                  (const char **) paths,
                  migrate_path,
                  &defaults, &defaults_size,
                  &values, &values_size,
                  &locks, &locks_size);

  if (defaults_size != 0 &&
      !flatpak_bwrap_add_args_data (bwrap,
                                    "dconf-defaults",
                                    defaults, defaults_size,
                                    "/etc/glib-2.0/settings/defaults",
                                    error))
    return FALSE;

  if (locks_size != 0 &&
      !flatpak_bwrap_add_args_data (bwrap,
                                    "dconf-locks",
                                    locks, locks_size,
                                    "/etc/glib-2.0/settings/locks",
                                    error))
    return FALSE;

  /* We do a one-time conversion of existing dconf settings to a keyfile.
   * Only do that once the app stops requesting dconf access.
   */
  if (migrate_path)
    {
      g_autofree char *filename = NULL;

      filename = g_build_filename (g_get_home_dir (),
                                   ".var/app", app_id,
                                   "config/glib-2.0/settings/keyfile",
                                   NULL);

      g_debug ("writing D-Conf values to %s", filename);

      if (values_size != 0 && !g_file_test (filename, G_FILE_TEST_EXISTS))
        {
          g_autofree char *dir = g_path_get_dirname (filename);

          if (g_mkdir_with_parents (dir, 0700) == -1)
            {
              g_warning ("failed creating dirs for %s", filename);
              return FALSE;
            }

          if (!g_file_set_contents (filename, values, values_size, error))
            {
              g_warning ("failed writing %s", filename);
              return FALSE;
            }
        }
    }

  return TRUE;
}