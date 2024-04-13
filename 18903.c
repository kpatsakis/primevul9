on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
  A11yBusLauncher *app = user_data;
  GError *error;
  guint registration_id;
  
  if (connection == NULL)
    {
      g_main_loop_quit (app->loop);
      return;
    }
  app->session_bus = connection;

  if (app->launch_immediately)
    {
      ensure_a11y_bus (app);
      if (app->state == A11Y_BUS_STATE_ERROR)
        {
          g_main_loop_quit (app->loop);
          return;
        }
    }

  error = NULL;
  registration_id = g_dbus_connection_register_object (connection,
                                                       "/org/a11y/bus",
                                                       introspection_data->interfaces[0],
                                                       &bus_vtable,
                                                       _global_app,
                                                       NULL,
                                                       &error);
  if (registration_id == 0)
    {
      g_error ("%s", error->message);
      g_clear_error (&error);
    }

  g_dbus_connection_register_object (connection,
                                     "/org/a11y/bus",
                                     introspection_data->interfaces[1],
                                     &status_vtable,
                                     _global_app,
                                     NULL,
                                     NULL);
}