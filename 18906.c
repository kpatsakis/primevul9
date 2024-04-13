handle_a11y_enabled_change (A11yBusLauncher *app, gboolean enabled,
                               gboolean notify_gsettings)
{
  GVariantBuilder builder;
  GVariantBuilder invalidated_builder;

  if (enabled == app->a11y_enabled)
    return;

  app->a11y_enabled = enabled;

  if (notify_gsettings && app->interface_schema)
    {
      g_settings_set_boolean (app->interface_schema, "toolkit-accessibility",
                              enabled);
      g_settings_sync ();
    }

  g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);
  g_variant_builder_init (&invalidated_builder, G_VARIANT_TYPE ("as"));
  g_variant_builder_add (&builder, "{sv}", "IsEnabled",
                         g_variant_new_boolean (enabled));

  g_dbus_connection_emit_signal (app->session_bus, NULL, "/org/a11y/bus",
                                 "org.freedesktop.DBus.Properties",
                                 "PropertiesChanged",
                                 g_variant_new ("(sa{sv}as)", "org.a11y.Status",
                                                &builder,
                                                &invalidated_builder),
                                 NULL);

  g_variant_builder_clear (&builder);
  g_variant_builder_clear (&invalidated_builder);
}