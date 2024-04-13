static int nwfilterConnectIsSecure(virConnectPtr conn G_GNUC_UNUSED)
{
    /* Trivially secure, since always inside the daemon */
    return 1;
}