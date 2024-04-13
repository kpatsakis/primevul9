respond_to_end_session (GDBusProxy *proxy)
{
  GVariant *parameters;

  parameters = g_variant_new ("(bs)", TRUE, "");

  g_dbus_proxy_call (proxy,
                     "EndSessionResponse", parameters,
                     G_DBUS_CALL_FLAGS_NONE,
                     -1, NULL, NULL, NULL);
}