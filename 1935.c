flatpak_run_add_system_dbus_args (FlatpakBwrap   *app_bwrap,
                                  FlatpakBwrap   *proxy_arg_bwrap,
                                  FlatpakContext *context,
                                  FlatpakRunFlags flags)
{
  gboolean unrestricted, no_proxy;
  const char *dbus_address = g_getenv ("DBUS_SYSTEM_BUS_ADDRESS");
  g_autofree char *real_dbus_address = NULL;
  g_autofree char *dbus_system_socket = NULL;

  unrestricted = (context->sockets & FLATPAK_CONTEXT_SOCKET_SYSTEM_BUS) != 0;
  if (unrestricted)
    g_debug ("Allowing system-dbus access");

  no_proxy = (flags & FLATPAK_RUN_FLAG_NO_SYSTEM_BUS_PROXY) != 0;

  if (dbus_address != NULL)
    dbus_system_socket = extract_unix_path_from_dbus_address (dbus_address);
  else if (g_file_test ("/var/run/dbus/system_bus_socket", G_FILE_TEST_EXISTS))
    dbus_system_socket = g_strdup ("/var/run/dbus/system_bus_socket");

  if (dbus_system_socket != NULL && unrestricted)
    {
      flatpak_bwrap_add_args (app_bwrap,
                              "--ro-bind", dbus_system_socket, "/run/dbus/system_bus_socket",
                              NULL);
      flatpak_bwrap_set_env (app_bwrap, "DBUS_SYSTEM_BUS_ADDRESS", "unix:path=/run/dbus/system_bus_socket", TRUE);

      return TRUE;
    }
  else if (!no_proxy && flatpak_context_get_needs_system_bus_proxy (context))
    {
      g_autofree char *proxy_socket = create_proxy_socket ("system-bus-proxy-XXXXXX");

      if (proxy_socket == NULL)
        return FALSE;

      if (dbus_address)
        real_dbus_address = g_strdup (dbus_address);
      else
        real_dbus_address = g_strdup_printf ("unix:path=%s", dbus_system_socket);

      flatpak_bwrap_add_args (proxy_arg_bwrap, real_dbus_address, proxy_socket, NULL);

      if (!unrestricted)
        flatpak_context_add_bus_filters (context, NULL, FALSE, flags & FLATPAK_RUN_FLAG_SANDBOX, proxy_arg_bwrap);

      if ((flags & FLATPAK_RUN_FLAG_LOG_SYSTEM_BUS) != 0)
        flatpak_bwrap_add_args (proxy_arg_bwrap, "--log", NULL);

      flatpak_bwrap_add_args (app_bwrap,
                              "--ro-bind", proxy_socket, "/run/dbus/system_bus_socket",
                              NULL);
      flatpak_bwrap_set_env (app_bwrap, "DBUS_SYSTEM_BUS_ADDRESS", "unix:path=/run/dbus/system_bus_socket", TRUE);

      return TRUE;
    }
  return FALSE;
}