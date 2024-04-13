name_appeared_handler (GDBusConnection *connection,
                       const gchar     *name,
                       const gchar     *name_owner,
                       gpointer         user_data)
{
  A11yBusLauncher *app = user_data;

  register_client (app);
}