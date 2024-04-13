nwfilterDriverRemoveDBusMatches(void)
{
    GDBusConnection *sysbus;

    sysbus = virGDBusGetSystemBus();
    if (sysbus) {
        g_dbus_connection_signal_unsubscribe(sysbus, restartID);
        g_dbus_connection_signal_unsubscribe(sysbus, reloadID);
    }
}