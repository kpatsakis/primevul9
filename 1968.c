add_tzdata_args (FlatpakBwrap *bwrap,
                 GFile *runtime_files)
{
  g_autofree char *raw_timezone = flatpak_get_timezone ();
  g_autofree char *timezone_content = g_strdup_printf ("%s\n", raw_timezone);
  g_autofree char *localtime_content = g_strconcat ("../usr/share/zoneinfo/", raw_timezone, NULL);
  g_autoptr(GFile) runtime_zoneinfo = NULL;

  if (runtime_files)
    runtime_zoneinfo = g_file_resolve_relative_path (runtime_files, "share/zoneinfo");

  /* Check for runtime /usr/share/zoneinfo */
  if (runtime_zoneinfo != NULL && g_file_query_exists (runtime_zoneinfo, NULL))
    {
      /* Check for host /usr/share/zoneinfo */
      if (g_file_test ("/usr/share/zoneinfo", G_FILE_TEST_IS_DIR))
        {
          /* Here we assume the host timezone file exist in the host data */
          flatpak_bwrap_add_args (bwrap,
                                  "--ro-bind", "/usr/share/zoneinfo", "/usr/share/zoneinfo",
                                  "--symlink", localtime_content, "/etc/localtime",
                                  NULL);
        }
      else
        {
          g_autoptr(GFile) runtime_tzfile = g_file_resolve_relative_path (runtime_zoneinfo, raw_timezone);

          /* Check if host timezone file exist in the runtime tzdata */
          if (g_file_query_exists (runtime_tzfile, NULL))
            flatpak_bwrap_add_args (bwrap,
                                    "--symlink", localtime_content, "/etc/localtime",
                                    NULL);
        }
    }

  flatpak_bwrap_add_args_data (bwrap, "timezone",
                               timezone_content, -1, "/etc/timezone",
                               NULL);
}