client_proxy_ready_cb (GObject      *source_object,
                       GAsyncResult *res,
                       gpointer      user_data)
{
  A11yBusLauncher *app = user_data;
  GError *error = NULL;

  app->client_proxy = g_dbus_proxy_new_for_bus_finish (res, &error);

  if (error != NULL)
    {
      g_warning ("Failed to get a client proxy: %s", error->message);
      g_error_free (error);

      return;
    }

  g_signal_connect (app->client_proxy, "g-signal",
                    G_CALLBACK (g_signal_cb), app);
}