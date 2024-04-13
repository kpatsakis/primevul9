nwfilterDriverInstallDBusMatches(GDBusConnection *sysbus)
{
    restartID = g_dbus_connection_signal_subscribe(sysbus,
                                                   NULL,
                                                   "org.freedesktop.DBus",
                                                   "NameOwnerChanged",
                                                   NULL,
                                                   "org.fedoraproject.FirewallD1",
                                                   G_DBUS_SIGNAL_FLAGS_NONE,
                                                   nwfilterFirewalldDBusSignalCallback,
                                                   NULL,
                                                   NULL);
    reloadID = g_dbus_connection_signal_subscribe(sysbus,
                                                  NULL,
                                                  "org.fedoraproject.FirewallD1",
                                                  "Reloaded",
                                                  NULL,
                                                  NULL,
                                                  G_DBUS_SIGNAL_FLAGS_NONE,
                                                  nwfilterFirewalldDBusSignalCallback,
                                                  NULL,
                                                  NULL);
}