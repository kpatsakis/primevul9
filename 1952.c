flatpak_run_setup_usr_links (FlatpakBwrap *bwrap,
                             GFile        *runtime_files,
                             const char   *sysroot)
{
  int i;

  if (runtime_files == NULL)
    return;

  for (i = 0; flatpak_abs_usrmerged_dirs[i] != NULL; i++)
    {
      const char *subdir = flatpak_abs_usrmerged_dirs[i];
      g_autoptr(GFile) runtime_subdir = NULL;

      g_assert (subdir[0] == '/');
      /* Skip the '/' when using as a subdirectory of the runtime */
      runtime_subdir = g_file_get_child (runtime_files, subdir + 1);

      if (g_file_query_exists (runtime_subdir, NULL))
        {
          g_autofree char *link = g_strconcat ("usr", subdir, NULL);
          g_autofree char *create = NULL;

          if (sysroot != NULL)
            create = g_strconcat (sysroot, subdir, NULL);
          else
            create = g_strdup (subdir);

          flatpak_bwrap_add_args (bwrap,
                                  "--symlink", link, create,
                                  NULL);
        }
      else
        {
          g_debug ("%s does not exist",
                   flatpak_file_get_path_cached (runtime_subdir));
        }
    }
}