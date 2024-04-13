on_name_lost (GDBusConnection *connection,
              const gchar     *name,
              gpointer         user_data)
{
  A11yBusLauncher *app = user_data;
  if (app->session_bus == NULL
      && connection == NULL
      && app->a11y_launch_error_message == NULL)
    app->a11y_launch_error_message = g_strdup ("Failed to connect to session bus");
  g_main_loop_quit (app->loop);
}