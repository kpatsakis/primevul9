flatpak_run_add_environment_args (FlatpakBwrap    *bwrap,
                                  const char      *app_info_path,
                                  FlatpakRunFlags  flags,
                                  const char      *app_id,
                                  FlatpakContext  *context,
                                  GFile           *app_id_dir,
                                  GPtrArray       *previous_app_id_dirs,
                                  int              per_app_dir_lock_fd,
                                  FlatpakExports **exports_out,
                                  GCancellable    *cancellable,
                                  GError         **error)
{
  g_autoptr(GError) my_error = NULL;
  g_autoptr(FlatpakExports) exports = NULL;
  g_autoptr(FlatpakBwrap) proxy_arg_bwrap = flatpak_bwrap_new (flatpak_bwrap_empty_env);
  g_autofree char *xdg_dirs_conf = NULL;
  gboolean has_wayland = FALSE;
  gboolean allow_x11 = FALSE;
  gboolean home_access = FALSE;
  gboolean sandboxed = (flags & FLATPAK_RUN_FLAG_SANDBOX) != 0;

  if ((context->shares & FLATPAK_CONTEXT_SHARED_IPC) == 0)
    {
      g_debug ("Disallowing ipc access");
      flatpak_bwrap_add_args (bwrap, "--unshare-ipc", NULL);
    }

  if ((context->shares & FLATPAK_CONTEXT_SHARED_NETWORK) == 0)
    {
      g_debug ("Disallowing network access");
      flatpak_bwrap_add_args (bwrap, "--unshare-net", NULL);
    }

  if (context->devices & FLATPAK_CONTEXT_DEVICE_ALL)
    {
      flatpak_bwrap_add_args (bwrap,
                              "--dev-bind", "/dev", "/dev",
                              NULL);
      /* Don't expose the host /dev/shm, just the device nodes, unless explicitly allowed */
      if (g_file_test ("/dev/shm", G_FILE_TEST_IS_DIR))
        {
          if (context->devices & FLATPAK_CONTEXT_DEVICE_SHM)
            {
              /* Don't do anything special: include shm in the
               * shared /dev. The host and all sandboxes and subsandboxes
               * all share /dev/shm */
            }
          else if ((context->features & FLATPAK_CONTEXT_FEATURE_PER_APP_DEV_SHM)
                   && per_app_dir_lock_fd >= 0)
            {
              g_autofree char *shared_dev_shm = NULL;

              /* The host and the original sandbox have separate /dev/shm,
               * but we want other instances to be able to share /dev/shm with
               * the first sandbox (except for subsandboxes run with
               * flatpak-spawn --sandbox, which will have their own). */
              if (!flatpak_instance_ensure_per_app_dev_shm (app_id,
                                                            per_app_dir_lock_fd,
                                                            &shared_dev_shm,
                                                            error))
                return FALSE;

              flatpak_bwrap_add_args (bwrap,
                                      "--bind", shared_dev_shm, "/dev/shm",
                                      NULL);
            }
          else
            {
              /* The host, the original sandbox and each subsandbox
               * each have a separate /dev/shm. */
              flatpak_bwrap_add_args (bwrap,
                                      "--tmpfs", "/dev/shm",
                                      NULL);
            }
        }
      else if (g_file_test ("/dev/shm", G_FILE_TEST_IS_SYMLINK))
        {
          g_autofree char *link = flatpak_readlink ("/dev/shm", NULL);

          /* On debian (with sysv init) the host /dev/shm is a symlink to /run/shm, so we can't
             mount on top of it. */
          if (g_strcmp0 (link, "/run/shm") == 0)
            {
              if (context->devices & FLATPAK_CONTEXT_DEVICE_SHM &&
                  g_file_test ("/run/shm", G_FILE_TEST_IS_DIR))
                {
                  flatpak_bwrap_add_args (bwrap,
                                          "--bind", "/run/shm", "/run/shm",
                                          NULL);
                }
              else if ((context->features & FLATPAK_CONTEXT_FEATURE_PER_APP_DEV_SHM)
                       && per_app_dir_lock_fd >= 0)
                {
                  g_autofree char *shared_dev_shm = NULL;

                  /* The host and the original sandbox have separate /dev/shm,
                   * but we want other instances to be able to share /dev/shm,
                   * except for flatpak-spawn --subsandbox. */
                  if (!flatpak_instance_ensure_per_app_dev_shm (app_id,
                                                                per_app_dir_lock_fd,
                                                                &shared_dev_shm,
                                                                error))
                    return FALSE;

                  flatpak_bwrap_add_args (bwrap,
                                          "--bind", shared_dev_shm, "/run/shm",
                                          NULL);
                }
              else
                {
                  flatpak_bwrap_add_args (bwrap,
                                          "--dir", "/run/shm",
                                          NULL);
                }
            }
          else
            g_warning ("Unexpected /dev/shm symlink %s", link);
        }
    }
  else
    {
      flatpak_bwrap_add_args (bwrap,
                              "--dev", "/dev",
                              NULL);
      if (context->devices & FLATPAK_CONTEXT_DEVICE_DRI)
        {
          g_debug ("Allowing dri access");
          int i;
          char *dri_devices[] = {
            "/dev/dri",
            /* mali */
            "/dev/mali",
            "/dev/mali0",
            "/dev/umplock",
            /* nvidia */
            "/dev/nvidiactl",
            "/dev/nvidia-modeset",
            /* nvidia OpenCL/CUDA */
            "/dev/nvidia-uvm",
            "/dev/nvidia-uvm-tools",
          };

          for (i = 0; i < G_N_ELEMENTS (dri_devices); i++)
            {
              if (g_file_test (dri_devices[i], G_FILE_TEST_EXISTS))
                flatpak_bwrap_add_args (bwrap, "--dev-bind", dri_devices[i], dri_devices[i], NULL);
            }

          /* Each Nvidia card gets its own device.
             This is a fairly arbitrary limit but ASUS sells mining boards supporting 20 in theory. */
          char nvidia_dev[14]; /* /dev/nvidia plus up to 2 digits */
          for (i = 0; i < 20; i++)
            {
              g_snprintf (nvidia_dev, sizeof (nvidia_dev), "/dev/nvidia%d", i);
              if (g_file_test (nvidia_dev, G_FILE_TEST_EXISTS))
                flatpak_bwrap_add_args (bwrap, "--dev-bind", nvidia_dev, nvidia_dev, NULL);
            }
        }

      if (context->devices & FLATPAK_CONTEXT_DEVICE_KVM)
        {
          g_debug ("Allowing kvm access");
          if (g_file_test ("/dev/kvm", G_FILE_TEST_EXISTS))
            flatpak_bwrap_add_args (bwrap, "--dev-bind", "/dev/kvm", "/dev/kvm", NULL);
        }

      if (context->devices & FLATPAK_CONTEXT_DEVICE_SHM)
        {
          /* This is a symlink to /run/shm on debian, so bind to real target */
          g_autofree char *real_dev_shm = realpath ("/dev/shm", NULL);

          g_debug ("Allowing /dev/shm access (as %s)", real_dev_shm);
          if (real_dev_shm != NULL)
              flatpak_bwrap_add_args (bwrap, "--bind", real_dev_shm, "/dev/shm", NULL);
        }
      else if ((context->features & FLATPAK_CONTEXT_FEATURE_PER_APP_DEV_SHM)
               && per_app_dir_lock_fd >= 0)
        {
          g_autofree char *shared_dev_shm = NULL;

          if (!flatpak_instance_ensure_per_app_dev_shm (app_id,
                                                        per_app_dir_lock_fd,
                                                        &shared_dev_shm,
                                                        error))
            return FALSE;

          flatpak_bwrap_add_args (bwrap,
                                  "--bind", shared_dev_shm, "/dev/shm",
                                  NULL);
        }
    }

  exports = flatpak_context_get_exports_full (context,
                                              app_id_dir, previous_app_id_dirs,
                                              TRUE, TRUE,
                                              &xdg_dirs_conf, &home_access);

  if (flatpak_exports_path_is_visible (exports, "/tmp"))
    {
      /* The original sandbox and any subsandboxes are both already
       * going to share /tmp with the host, so by transitivity they will
       * also share it with each other, and with all other instances. */
    }
  else if (per_app_dir_lock_fd >= 0 && !sandboxed)
    {
      g_autofree char *shared_tmp = NULL;

      /* The host and the original sandbox have separate /tmp,
       * but we want other instances to be able to share /tmp with the
       * first sandbox, unless they were created by
       * flatpak-spawn --sandbox.
       *
       * In apply_extra and `flatpak build`, per_app_dir_lock_fd is
       * negative and we skip this. */
      if (!flatpak_instance_ensure_per_app_tmp (app_id,
                                                per_app_dir_lock_fd,
                                                &shared_tmp,
                                                error))
        return FALSE;

      flatpak_bwrap_add_args (bwrap,
                              "--bind", shared_tmp, "/tmp",
                              NULL);
    }

  flatpak_context_append_bwrap_filesystem (context, bwrap, app_id, app_id_dir,
                                           exports, xdg_dirs_conf, home_access);

  if (context->sockets & FLATPAK_CONTEXT_SOCKET_WAYLAND)
    {
      g_debug ("Allowing wayland access");
      has_wayland = flatpak_run_add_wayland_args (bwrap);
    }

  if ((context->sockets & FLATPAK_CONTEXT_SOCKET_FALLBACK_X11) != 0)
    allow_x11 = !has_wayland;
  else
    allow_x11 = (context->sockets & FLATPAK_CONTEXT_SOCKET_X11) != 0;

  flatpak_run_add_x11_args (bwrap, allow_x11);

  if (context->sockets & FLATPAK_CONTEXT_SOCKET_SSH_AUTH)
    {
      flatpak_run_add_ssh_args (bwrap);
    }

  if (context->sockets & FLATPAK_CONTEXT_SOCKET_PULSEAUDIO)
    {
      g_debug ("Allowing pulseaudio access");
      flatpak_run_add_pulseaudio_args (bwrap);
    }

  if (context->sockets & FLATPAK_CONTEXT_SOCKET_PCSC)
    {
      flatpak_run_add_pcsc_args (bwrap);
    }

  if (context->sockets & FLATPAK_CONTEXT_SOCKET_CUPS)
    {
      flatpak_run_add_cups_args (bwrap);
    }

  flatpak_run_add_session_dbus_args (bwrap, proxy_arg_bwrap, context, flags, app_id);
  flatpak_run_add_system_dbus_args (bwrap, proxy_arg_bwrap, context, flags);
  flatpak_run_add_a11y_dbus_args (bwrap, proxy_arg_bwrap, context, flags);

  /* Must run this before spawning the dbus proxy, to ensure it
     ends up in the app cgroup */
  if (!flatpak_run_in_transient_unit (app_id, &my_error))
    {
      /* We still run along even if we don't get a cgroup, as nothing
         really depends on it. Its just nice to have */
      g_debug ("Failed to run in transient scope: %s", my_error->message);
      g_clear_error (&my_error);
    }

  if (!flatpak_bwrap_is_empty (proxy_arg_bwrap) &&
      !start_dbus_proxy (bwrap, proxy_arg_bwrap, app_info_path, error))
    return FALSE;

  if (exports_out)
    *exports_out = g_steal_pointer (&exports);

  return TRUE;
}