gsettings_key_changed (GSettings *gsettings, const gchar *key, void *user_data)
{
  gboolean new_val = g_settings_get_boolean (gsettings, key);
  A11yBusLauncher *app = user_data;

  if (!strcmp (key, "toolkit-accessibility"))
    handle_a11y_enabled_change (_global_app, new_val, FALSE);
  else if (!strcmp (key, "screen-reader-enabled"))
    handle_screen_reader_enabled_change (_global_app, new_val, FALSE);
}