flatpak_run_add_a11y_dbus_args (FlatpakBwrap   *app_bwrap,
                                FlatpakBwrap   *proxy_arg_bwrap,
                                FlatpakContext *context,
                                FlatpakRunFlags flags)
{
  static const char sandbox_socket_path[] = "/run/flatpak/at-spi-bus";
  static const char sandbox_dbus_address[] = "unix:path=/run/flatpak/at-spi-bus";
  g_autoptr(GDBusConnection) session_bus = NULL;
  g_autofree char *a11y_address = NULL;
  g_autoptr(GError) local_error = NULL;
  g_autoptr(GDBusMessage) reply = NULL;
  g_autoptr(GDBusMessage) msg = NULL;
  g_autofree char *proxy_socket = NULL;

  if ((flags & FLATPAK_RUN_FLAG_NO_A11Y_BUS_PROXY) != 0)
    return FALSE;

  session_bus = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, NULL);
  if (session_bus == NULL)
    return FALSE;

  msg = g_dbus_message_new_method_call ("org.a11y.Bus", "/org/a11y/bus", "org.a11y.Bus", "GetAddress");
  g_dbus_message_set_body (msg, g_variant_new ("()"));
  reply =
    g_dbus_connection_send_message_with_reply_sync (session_bus, msg,
                                                    G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                    30000,
                                                    NULL,
                                                    NULL,
                                                    NULL);
  if (reply)
    {
      if (g_dbus_message_to_gerror (reply, &local_error))
        {
          if (!g_error_matches (local_error, G_DBUS_ERROR, G_DBUS_ERROR_SERVICE_UNKNOWN))
            g_message ("Can't find a11y bus: %s", local_error->message);
        }
      else
        {
          g_variant_get (g_dbus_message_get_body (reply),
                         "(s)", &a11y_address);
        }
    }

  if (!a11y_address)
    return FALSE;

  proxy_socket = create_proxy_socket ("a11y-bus-proxy-XXXXXX");
  if (proxy_socket == NULL)
    return FALSE;

  flatpak_bwrap_add_args (proxy_arg_bwrap,
                          a11y_address,
                          proxy_socket, "--filter", "--sloppy-names",
                          "--call=org.a11y.atspi.Registry=org.a11y.atspi.Socket.Embed@/org/a11y/atspi/accessible/root",
                          "--call=org.a11y.atspi.Registry=org.a11y.atspi.Socket.Unembed@/org/a11y/atspi/accessible/root",
                          "--call=org.a11y.atspi.Registry=org.a11y.atspi.Registry.GetRegisteredEvents@/org/a11y/atspi/registry",
                          "--call=org.a11y.atspi.Registry=org.a11y.atspi.DeviceEventController.GetKeystrokeListeners@/org/a11y/atspi/registry/deviceeventcontroller",
                          "--call=org.a11y.atspi.Registry=org.a11y.atspi.DeviceEventController.GetDeviceEventListeners@/org/a11y/atspi/registry/deviceeventcontroller",
                          "--call=org.a11y.atspi.Registry=org.a11y.atspi.DeviceEventController.NotifyListenersSync@/org/a11y/atspi/registry/deviceeventcontroller",
                          "--call=org.a11y.atspi.Registry=org.a11y.atspi.DeviceEventController.NotifyListenersAsync@/org/a11y/atspi/registry/deviceeventcontroller",
                          NULL);

  if ((flags & FLATPAK_RUN_FLAG_LOG_A11Y_BUS) != 0)
    flatpak_bwrap_add_args (proxy_arg_bwrap, "--log", NULL);

  flatpak_bwrap_add_args (app_bwrap,
                          "--ro-bind", proxy_socket, sandbox_socket_path,
                          NULL);
  flatpak_bwrap_set_env (app_bwrap, "AT_SPI_BUS_ADDRESS", sandbox_dbus_address, TRUE);

  return TRUE;
}