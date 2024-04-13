regenerate_ld_cache (GPtrArray    *base_argv_array,
                     GArray       *base_fd_array,
                     GFile        *app_id_dir,
                     const char   *checksum,
                     GFile        *runtime_files,
                     gboolean      generate_ld_so_conf,
                     GCancellable *cancellable,
                     GError      **error)
{
  g_autoptr(FlatpakBwrap) bwrap = NULL;
  g_autoptr(GArray) combined_fd_array = NULL;
  g_autoptr(GFile) ld_so_cache = NULL;
  g_autoptr(GFile) ld_so_cache_tmp = NULL;
  g_autofree char *sandbox_cache_path = NULL;
  g_autofree char *tmp_basename = NULL;
  g_auto(GStrv) minimal_envp = NULL;
  g_autofree char *commandline = NULL;
  int exit_status;
  glnx_autofd int ld_so_fd = -1;
  g_autoptr(GFile) ld_so_dir = NULL;

  if (app_id_dir)
    ld_so_dir = g_file_get_child (app_id_dir, ".ld.so");
  else
    {
      g_autoptr(GFile) base_dir = g_file_new_for_path (g_get_user_cache_dir ());
      ld_so_dir = g_file_resolve_relative_path (base_dir, "flatpak/ld.so");
    }

  ld_so_cache = g_file_get_child (ld_so_dir, checksum);
  ld_so_fd = open (flatpak_file_get_path_cached (ld_so_cache), O_RDONLY);
  if (ld_so_fd >= 0)
    return glnx_steal_fd (&ld_so_fd);

  g_debug ("Regenerating ld.so.cache %s", flatpak_file_get_path_cached (ld_so_cache));

  if (!flatpak_mkdir_p (ld_so_dir, cancellable, error))
    return FALSE;

  minimal_envp = flatpak_run_get_minimal_env (FALSE, FALSE);
  bwrap = flatpak_bwrap_new (minimal_envp);

  flatpak_bwrap_append_args (bwrap, base_argv_array);

  flatpak_run_setup_usr_links (bwrap, runtime_files, NULL);

  if (generate_ld_so_conf)
    {
      if (!add_ld_so_conf (bwrap, error))
        return -1;
    }
  else
    flatpak_bwrap_add_args (bwrap,
                            "--symlink", "../usr/etc/ld.so.conf", "/etc/ld.so.conf",
                            NULL);

  tmp_basename = g_strconcat (checksum, ".XXXXXX", NULL);
  glnx_gen_temp_name (tmp_basename);

  sandbox_cache_path = g_build_filename ("/run/ld-so-cache-dir", tmp_basename, NULL);
  ld_so_cache_tmp = g_file_get_child (ld_so_dir, tmp_basename);

  flatpak_bwrap_add_args (bwrap,
                          "--unshare-pid",
                          "--unshare-ipc",
                          "--unshare-net",
                          "--proc", "/proc",
                          "--dev", "/dev",
                          "--bind", flatpak_file_get_path_cached (ld_so_dir), "/run/ld-so-cache-dir",
                          NULL);
  flatpak_bwrap_sort_envp (bwrap);
  flatpak_bwrap_envp_to_args (bwrap);

  if (!flatpak_bwrap_bundle_args (bwrap, 1, -1, FALSE, error))
    return -1;

  flatpak_bwrap_add_args (bwrap,
                          "ldconfig", "-X", "-C", sandbox_cache_path, NULL);

  flatpak_bwrap_finish (bwrap);

  commandline = flatpak_quote_argv ((const char **) bwrap->argv->pdata, -1);
  g_debug ("Running: '%s'", commandline);

  combined_fd_array = g_array_new (FALSE, TRUE, sizeof (int));
  g_array_append_vals (combined_fd_array, base_fd_array->data, base_fd_array->len);
  g_array_append_vals (combined_fd_array, bwrap->fds->data, bwrap->fds->len);

  /* We use LEAVE_DESCRIPTORS_OPEN to work around dead-lock, see flatpak_close_fds_workaround */
  if (!g_spawn_sync (NULL,
                     (char **) bwrap->argv->pdata,
                     bwrap->envp,
                     G_SPAWN_SEARCH_PATH | G_SPAWN_LEAVE_DESCRIPTORS_OPEN,
                     flatpak_bwrap_child_setup_cb, combined_fd_array,
                     NULL, NULL,
                     &exit_status,
                     error))
    return -1;

  if (!WIFEXITED (exit_status) || WEXITSTATUS (exit_status) != 0)
    {
      flatpak_fail_error (error, FLATPAK_ERROR_SETUP_FAILED,
                          _("ldconfig failed, exit status %d"), exit_status);
      return -1;
    }

  ld_so_fd = open (flatpak_file_get_path_cached (ld_so_cache_tmp), O_RDONLY);
  if (ld_so_fd < 0)
    {
      flatpak_fail_error (error, FLATPAK_ERROR_SETUP_FAILED, _("Can't open generated ld.so.cache"));
      return -1;
    }

  if (app_id_dir == NULL)
    {
      /* For runs without an app id dir we always regenerate the ld.so.cache */
      unlink (flatpak_file_get_path_cached (ld_so_cache_tmp));
    }
  else
    {
      g_autoptr(GFile) active = g_file_get_child (ld_so_dir, "active");

      /* For app-dirs we keep one checksum alive, by pointing the active symlink to it */

      /* Rename to known name, possibly overwriting existing ref if race */
      if (rename (flatpak_file_get_path_cached (ld_so_cache_tmp), flatpak_file_get_path_cached (ld_so_cache)) == -1)
        {
          glnx_set_error_from_errno (error);
          return -1;
        }

      if (!flatpak_switch_symlink_and_remove (flatpak_file_get_path_cached (active),
                                              checksum, error))
        return -1;
    }

  return glnx_steal_fd (&ld_so_fd);
}