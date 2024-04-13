g_signal_cb (GDBusProxy *proxy,
             gchar      *sender_name,
             gchar      *signal_name,
             GVariant   *parameters,
             gpointer    user_data)
{
  A11yBusLauncher *app = user_data;

  if (g_strcmp0 (signal_name, "QueryEndSession") == 0)
    respond_to_end_session (proxy);
  else if (g_strcmp0 (signal_name, "EndSession") == 0)
    respond_to_end_session (proxy);
  else if (g_strcmp0 (signal_name, "Stop") == 0)
    g_main_loop_quit (app->loop);
}