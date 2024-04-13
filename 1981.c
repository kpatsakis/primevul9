flatpak_run_add_x11_args (FlatpakBwrap *bwrap,
                          gboolean      allowed)
{
  g_autofree char *x11_socket = NULL;
  const char *display;

  /* Always cover /tmp/.X11-unix, that way we never see the host one in case
   * we have access to the host /tmp. If you request X access we'll put the right
   * thing in this anyway.
   *
   * We need to be a bit careful here, because there are two situations in
   * which potentially hostile processes have access to /tmp and could
   * create symlinks, which in principle could cause us to create the
   * directory and mount the tmpfs at the target of the symlink instead
   * of in the intended place:
   *
   * - With --filesystem=/tmp, it's the host /tmp - but because of the
   *   special historical status of /tmp/.X11-unix, we can assume that
   *   it is pre-created by the host system before user code gets to run.
   *
   * - When /tmp is shared between all instances of the same app ID,
   *   in principle the app has control over what's in /tmp, but in
   *   practice it can't interfere with /tmp/.X11-unix, because we do
   *   this unconditionally - therefore by the time app code runs,
   *   /tmp/.X11-unix is already a mount point, meaning the app cannot
   *   rename or delete it.
   */
  flatpak_bwrap_add_args (bwrap,
                          "--tmpfs", "/tmp/.X11-unix",
                          NULL);

  if (!allowed)
    {
      flatpak_bwrap_unset_env (bwrap, "DISPLAY");
      return;
    }

  g_debug ("Allowing x11 access");

  display = g_getenv ("DISPLAY");
  if (display && display[0] == ':' && g_ascii_isdigit (display[1]))
    {
      const char *display_nr = &display[1];
      const char *display_nr_end = display_nr;
      g_autofree char *d = NULL;

      while (g_ascii_isdigit (*display_nr_end))
        display_nr_end++;

      d = g_strndup (display_nr, display_nr_end - display_nr);
      x11_socket = g_strdup_printf ("/tmp/.X11-unix/X%s", d);

      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", x11_socket, "/tmp/.X11-unix/X99",
                              NULL);
      flatpak_bwrap_set_env (bwrap, "DISPLAY", ":99.0", TRUE);

#ifdef ENABLE_XAUTH
      g_auto(GLnxTmpfile) xauth_tmpf  = { 0, };

      if (glnx_open_anonymous_tmpfile_full (O_RDWR | O_CLOEXEC, "/tmp", &xauth_tmpf, NULL))
        {
          FILE *output = fdopen (xauth_tmpf.fd, "wb");
          if (output != NULL)
            {
              /* fd is now owned by output, steal it from the tmpfile */
              int tmp_fd = dup (glnx_steal_fd (&xauth_tmpf.fd));
              if (tmp_fd != -1)
                {
                  static const char dest[] = "/run/flatpak/Xauthority";

                  write_xauth (d, output);
                  flatpak_bwrap_add_args_data_fd (bwrap, "--ro-bind-data", tmp_fd, dest);

                  flatpak_bwrap_set_env (bwrap, "XAUTHORITY", dest, TRUE);
                }

              fclose (output);

              if (tmp_fd != -1)
                lseek (tmp_fd, 0, SEEK_SET);
            }
        }
#endif
    }
  else
    {
      flatpak_bwrap_unset_env (bwrap, "DISPLAY");
    }
}