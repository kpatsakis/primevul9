on_name_acquired (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
  g_bus_watch_name (G_BUS_TYPE_SESSION,
                    "org.gnome.SessionManager",
                    G_BUS_NAME_WATCHER_FLAGS_NONE,
                    name_appeared_handler, NULL,
                    user_data, NULL);
}