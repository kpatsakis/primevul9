flatpak_run_add_session_dbus_args (FlatpakBwrap   *app_bwrap,
                                   FlatpakBwrap   *proxy_arg_bwrap,
                                   FlatpakContext *context,
                                   FlatpakRunFlags flags,
                                   const char     *app_id)
{
  static const char sandbox_socket_path[] = "/run/flatpak/bus";
  static const char sandbox_dbus_address[] = "unix:path=/run/flatpak/bus";
  gboolean unrestricted, no_proxy;
  const char *dbus_address = g_getenv ("DBUS_SESSION_BUS_ADDRESS");
  g_autofree char *dbus_session_socket = NULL;

  unrestricted = (context->sockets & FLATPAK_CONTEXT_SOCKET_SESSION_BUS) != 0;

  if (dbus_address != NULL)
    {
      dbus_session_socket = extract_unix_path_from_dbus_address (dbus_address);
    }
  else
    {
      g_autofree char *user_runtime_dir = flatpak_get_real_xdg_runtime_dir ();
      struct stat statbuf;

      dbus_session_socket = g_build_filename (user_runtime_dir, "bus", NULL);

      if (stat (dbus_session_socket, &statbuf) < 0
          || (statbuf.st_mode & S_IFMT) != S_IFSOCK
          || statbuf.st_uid != getuid ())
        return FALSE;
    }

  if (unrestricted)
    g_debug ("Allowing session-dbus access");

  no_proxy = (flags & FLATPAK_RUN_FLAG_NO_SESSION_BUS_PROXY) != 0;

  if (dbus_session_socket != NULL && unrestricted)
    {
      flatpak_bwrap_add_args (app_bwrap,
                              "--ro-bind", dbus_session_socket, sandbox_socket_path,
                              NULL);
      flatpak_bwrap_set_env (app_bwrap, "DBUS_SESSION_BUS_ADDRESS", sandbox_dbus_address, TRUE);
      flatpak_bwrap_add_runtime_dir_member (app_bwrap, "bus");

      return TRUE;
    }
  else if (!no_proxy && dbus_address != NULL)
    {
      g_autofree char *proxy_socket = create_proxy_socket ("session-bus-proxy-XXXXXX");

      if (proxy_socket == NULL)
        return FALSE;

      flatpak_bwrap_add_args (proxy_arg_bwrap, dbus_address, proxy_socket, NULL);

      if (!unrestricted)
        {
          flatpak_context_add_bus_filters (context, app_id, TRUE, flags & FLATPAK_RUN_FLAG_SANDBOX, proxy_arg_bwrap);

          /* Allow calling any interface+method on all portals, but only receive broadcasts under /org/desktop/portal */
          flatpak_bwrap_add_arg (proxy_arg_bwrap,
                                 "--call=org.freedesktop.portal.*=*");
          flatpak_bwrap_add_arg (proxy_arg_bwrap,
                                 "--broadcast=org.freedesktop.portal.*=@/org/freedesktop/portal/*");
        }

      if ((flags & FLATPAK_RUN_FLAG_LOG_SESSION_BUS) != 0)
        flatpak_bwrap_add_args (proxy_arg_bwrap, "--log", NULL);

      flatpak_bwrap_add_args (app_bwrap,
                              "--ro-bind", proxy_socket, sandbox_socket_path,
                              NULL);
      flatpak_bwrap_set_env (app_bwrap, "DBUS_SESSION_BUS_ADDRESS", sandbox_dbus_address, TRUE);
      flatpak_bwrap_add_runtime_dir_member (app_bwrap, "bus");

      return TRUE;
    }

  return FALSE;
}