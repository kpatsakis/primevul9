flatpak_run_add_app_info_args (FlatpakBwrap   *bwrap,
                               GFile          *app_files,
                               GVariant       *app_deploy_data,
                               const char     *app_extensions,
                               GFile          *runtime_files,
                               GVariant       *runtime_deploy_data,
                               const char     *runtime_extensions,
                               const char     *app_id,
                               const char     *app_branch,
                               const char     *runtime_ref,
                               GFile          *app_id_dir,
                               FlatpakContext *final_app_context,
                               FlatpakContext *cmdline_context,
                               gboolean        sandbox,
                               gboolean        build,
                               gboolean        devel,
                               char          **app_info_path_out,
                               char          **instance_id_host_dir_out,
                               GError        **error)
{
  g_autofree char *info_path = NULL;
  g_autofree char *bwrapinfo_path = NULL;
  int fd, fd2, fd3;
  g_autoptr(GKeyFile) keyfile = NULL;
  g_autofree char *runtime_path = NULL;
  g_autofree char *old_dest = g_strdup_printf ("/run/user/%d/flatpak-info", getuid ());
  g_auto(GStrv) runtime_ref_parts = g_strsplit (runtime_ref, "/", 0);
  const char *group;
  g_autofree char *instance_id = NULL;
  glnx_autofd int lock_fd = -1;
  g_autofree char *instance_id_host_dir = NULL;
  g_autofree char *instance_id_sandbox_dir = NULL;
  g_autofree char *instance_id_lock_file = NULL;
  g_autofree char *user_runtime_dir = flatpak_get_real_xdg_runtime_dir ();

  instance_id = flatpak_run_allocate_id (&lock_fd);
  if (instance_id == NULL)
    return flatpak_fail_error (error, FLATPAK_ERROR_SETUP_FAILED, _("Unable to allocate instance id"));

  instance_id_host_dir = g_build_filename (user_runtime_dir, ".flatpak", instance_id, NULL);
  instance_id_sandbox_dir = g_strdup_printf ("/run/user/%d/.flatpak/%s", getuid (), instance_id);
  instance_id_lock_file = g_build_filename (instance_id_sandbox_dir, ".ref", NULL);

  flatpak_bwrap_add_args (bwrap,
                          "--ro-bind",
                          instance_id_host_dir,
                          instance_id_sandbox_dir,
                          "--lock-file",
                          instance_id_lock_file,
                          NULL);
  /* Keep the .ref lock held until we've started bwrap to avoid races */
  flatpak_bwrap_add_noinherit_fd (bwrap, glnx_steal_fd (&lock_fd));

  info_path = g_build_filename (instance_id_host_dir, "info", NULL);

  keyfile = g_key_file_new ();

  if (app_files)
    group = FLATPAK_METADATA_GROUP_APPLICATION;
  else
    group = FLATPAK_METADATA_GROUP_RUNTIME;

  g_key_file_set_string (keyfile, group, FLATPAK_METADATA_KEY_NAME, app_id);
  g_key_file_set_string (keyfile, group, FLATPAK_METADATA_KEY_RUNTIME,
                         runtime_ref);

  g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                         FLATPAK_METADATA_KEY_INSTANCE_ID, instance_id);
  if (app_id_dir)
    {
      g_autofree char *instance_path = g_file_get_path (app_id_dir);
      g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                             FLATPAK_METADATA_KEY_INSTANCE_PATH, instance_path);
    }

  if (app_files)
    {
      g_autofree char *app_path = g_file_get_path (app_files);
      g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                             FLATPAK_METADATA_KEY_APP_PATH, app_path);
    }
  if (app_deploy_data)
    g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                           FLATPAK_METADATA_KEY_APP_COMMIT, flatpak_deploy_data_get_commit (app_deploy_data));
  if (app_extensions && *app_extensions != 0)
    g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                           FLATPAK_METADATA_KEY_APP_EXTENSIONS, app_extensions);
  runtime_path = g_file_get_path (runtime_files);
  g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                         FLATPAK_METADATA_KEY_RUNTIME_PATH, runtime_path);
  if (runtime_deploy_data)
    g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                           FLATPAK_METADATA_KEY_RUNTIME_COMMIT, flatpak_deploy_data_get_commit (runtime_deploy_data));
  if (runtime_extensions && *runtime_extensions != 0)
    g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                           FLATPAK_METADATA_KEY_RUNTIME_EXTENSIONS, runtime_extensions);
  if (app_branch != NULL)
    g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                           FLATPAK_METADATA_KEY_BRANCH, app_branch);
  g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                         FLATPAK_METADATA_KEY_ARCH, runtime_ref_parts[2]);

  g_key_file_set_string (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                         FLATPAK_METADATA_KEY_FLATPAK_VERSION, PACKAGE_VERSION);

  if ((final_app_context->sockets & FLATPAK_CONTEXT_SOCKET_SESSION_BUS) == 0)
    g_key_file_set_boolean (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                            FLATPAK_METADATA_KEY_SESSION_BUS_PROXY, TRUE);

  if ((final_app_context->sockets & FLATPAK_CONTEXT_SOCKET_SYSTEM_BUS) == 0)
    g_key_file_set_boolean (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                            FLATPAK_METADATA_KEY_SYSTEM_BUS_PROXY, TRUE);

  if (sandbox)
    g_key_file_set_boolean (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                            FLATPAK_METADATA_KEY_SANDBOX, TRUE);
  if (build)
    g_key_file_set_boolean (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                            FLATPAK_METADATA_KEY_BUILD, TRUE);
  if (devel)
    g_key_file_set_boolean (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                            FLATPAK_METADATA_KEY_DEVEL, TRUE);

  if (cmdline_context)
    {
      g_autoptr(GPtrArray) cmdline_args = g_ptr_array_new_with_free_func (g_free);
      flatpak_context_to_args (cmdline_context, cmdline_args);
      if (cmdline_args->len > 0)
        {
          g_key_file_set_string_list (keyfile, FLATPAK_METADATA_GROUP_INSTANCE,
                                      FLATPAK_METADATA_KEY_EXTRA_ARGS,
                                      (const char * const *) cmdline_args->pdata,
                                      cmdline_args->len);
        }
    }

  flatpak_context_save_metadata (final_app_context, TRUE, keyfile);

  if (!g_key_file_save_to_file (keyfile, info_path, error))
    return FALSE;

  /* We want to create a file on /.flatpak-info that the app cannot modify, which
     we do by creating a read-only bind mount. This way one can openat()
     /proc/$pid/root, and if that succeeds use openat via that to find the
     unfakable .flatpak-info file. However, there is a tiny race in that if
     you manage to open /proc/$pid/root, but then the pid dies, then
     every mount but the root is unmounted in the namespace, so the
     .flatpak-info will be empty. We fix this by first creating a real file
     with the real info in, then bind-mounting on top of that, the same info.
     This way even if the bind-mount is unmounted we can find the real data.
   */

  fd = open (info_path, O_RDONLY);
  if (fd == -1)
    {
      int errsv = errno;
      g_set_error (error, G_IO_ERROR, g_io_error_from_errno (errsv),
                   _("Failed to open flatpak-info file: %s"), g_strerror (errsv));
      return FALSE;
    }

  fd2 = open (info_path, O_RDONLY);
  if (fd2 == -1)
    {
      close (fd);
      int errsv = errno;
      g_set_error (error, G_IO_ERROR, g_io_error_from_errno (errsv),
                   _("Failed to open flatpak-info file: %s"), g_strerror (errsv));
      return FALSE;
    }

  flatpak_bwrap_add_args_data_fd (bwrap,
                                  "--file", fd, "/.flatpak-info");
  flatpak_bwrap_add_args_data_fd (bwrap,
                                  "--ro-bind-data", fd2, "/.flatpak-info");
  flatpak_bwrap_add_args (bwrap,
                          "--symlink", "../../../.flatpak-info", old_dest,
                          NULL);

  bwrapinfo_path = g_build_filename (instance_id_host_dir, "bwrapinfo.json", NULL);
  fd3 = open (bwrapinfo_path, O_RDWR | O_CREAT, 0644);
  if (fd3 == -1)
    {
      close (fd);
      close (fd2);
      int errsv = errno;
      g_set_error (error, G_IO_ERROR, g_io_error_from_errno (errsv),
                   _("Failed to open bwrapinfo.json file: %s"), g_strerror (errsv));
      return FALSE;
    }

  flatpak_bwrap_add_args_data_fd (bwrap, "--info-fd", fd3, NULL);

  if (app_info_path_out != NULL)
    *app_info_path_out = g_strdup_printf ("/proc/self/fd/%d", fd);

  if (instance_id_host_dir_out != NULL)
    *instance_id_host_dir_out = g_steal_pointer (&instance_id_host_dir);

  return TRUE;
}