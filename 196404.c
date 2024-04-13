nwfilterFirewalldDBusSignalCallback(GDBusConnection *connection G_GNUC_UNUSED,
                                    const char *senderName G_GNUC_UNUSED,
                                    const char *objectPath G_GNUC_UNUSED,
                                    const char *interfaceName G_GNUC_UNUSED,
                                    const char *signalName G_GNUC_UNUSED,
                                    GVariant *parameters G_GNUC_UNUSED,
                                    gpointer user_data G_GNUC_UNUSED)
{
    nwfilterStateReload();
}