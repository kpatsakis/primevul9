register_client (A11yBusLauncher *app)
{
  GDBusProxyFlags flags;
  GDBusProxy *sm_proxy;
  GError *error;
  const gchar *app_id;
  const gchar *autostart_id;
  gchar *client_startup_id;
  GVariant *parameters;
  GVariant *variant;
  gchar *object_path;

  flags = G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES |
          G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS;

  error = NULL;
  sm_proxy = g_dbus_proxy_new_sync (app->session_bus, flags, NULL,
                                    "org.gnome.SessionManager",
                                    "/org/gnome/SessionManager",
                                    "org.gnome.SessionManager",
                                    NULL, &error);

  if (error != NULL)
    {
      g_warning ("Failed to get session manager proxy: %s", error->message);
      g_error_free (error);

      return;
    }

  app_id = "at-spi-bus-launcher";
  autostart_id = g_getenv ("DESKTOP_AUTOSTART_ID");

  if (autostart_id != NULL)
    {
      client_startup_id = g_strdup (autostart_id);
      g_unsetenv ("DESKTOP_AUTOSTART_ID");
    }
  else
    {
      client_startup_id = g_strdup ("");
    }

  parameters = g_variant_new ("(ss)", app_id, client_startup_id);
  g_free (client_startup_id);

  error = NULL;
  variant = g_dbus_proxy_call_sync (sm_proxy,
                                    "RegisterClient", parameters,
                                    G_DBUS_CALL_FLAGS_NONE,
                                    -1, NULL, &error);

  g_object_unref (sm_proxy);

  if (error != NULL)
    {
      g_warning ("Failed to register client: %s", error->message);
      g_error_free (error);

      return;
    }

  g_variant_get (variant, "(o)", &object_path);
  g_variant_unref (variant);

  flags = G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES;
  g_dbus_proxy_new_for_bus (G_BUS_TYPE_SESSION, flags, NULL,
                            "org.gnome.SessionManager", object_path,
                            "org.gnome.SessionManager.ClientPrivate",
                            NULL, client_proxy_ready_cb, app);

  g_free (object_path);
}