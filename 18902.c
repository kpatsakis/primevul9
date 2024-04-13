handle_get_property  (GDBusConnection       *connection,
                      const gchar           *sender,
                      const gchar           *object_path,
                      const gchar           *interface_name,
                      const gchar           *property_name,
                    GError **error,
                    gpointer               user_data)
{
  A11yBusLauncher *app = user_data;

  if (g_strcmp0 (property_name, "IsEnabled") == 0)
    return g_variant_new ("b", app->a11y_enabled);
  else if (g_strcmp0 (property_name, "ScreenReaderEnabled") == 0)
    return g_variant_new ("b", app->screen_reader_enabled);
  else
    return NULL;
}