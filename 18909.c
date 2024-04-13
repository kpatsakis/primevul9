handle_method_call (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *interface_name,
                    const gchar           *method_name,
                    GVariant              *parameters,
                    GDBusMethodInvocation *invocation,
                    gpointer               user_data)
{
  A11yBusLauncher *app = user_data;

  if (g_strcmp0 (method_name, "GetAddress") == 0)
    {
      ensure_a11y_bus (app);
      if (app->a11y_bus_pid > 0)
        g_dbus_method_invocation_return_value (invocation,
                                               g_variant_new ("(s)", app->a11y_bus_address));
      else
        g_dbus_method_invocation_return_dbus_error (invocation,
                                                    "org.a11y.Bus.Error",
                                                    app->a11y_launch_error_message);
    }
}