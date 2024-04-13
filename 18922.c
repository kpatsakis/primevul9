handle_set_property  (GDBusConnection       *connection,
                      const gchar           *sender,
                      const gchar           *object_path,
                      const gchar           *interface_name,
                      const gchar           *property_name,
                      GVariant *value,
                    GError **error,
                    gpointer               user_data)
{
  A11yBusLauncher *app = user_data;
  const gchar *type = g_variant_get_type_string (value);
  gboolean enabled;
  
  if (g_strcmp0 (type, "b") != 0)
    {
      g_set_error (error, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS,
                       "org.a11y.Status.%s expects a boolean but got %s", property_name, type);
      return FALSE;
    }

  enabled = g_variant_get_boolean (value);

  if (g_strcmp0 (property_name, "IsEnabled") == 0)
    {
      handle_a11y_enabled_change (app, enabled, TRUE);
      return TRUE;
    }
  else if (g_strcmp0 (property_name, "ScreenReaderEnabled") == 0)
    {
      handle_screen_reader_enabled_change (app, enabled, TRUE);
      return TRUE;
    }
  else
    {
      g_set_error (error, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS,
                       "Unknown property '%s'", property_name);
      return FALSE;
    }
}