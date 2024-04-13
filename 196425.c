static int nwfilterConnectIsEncrypted(virConnectPtr conn G_GNUC_UNUSED)
{
    /* Not encrypted, but remote driver takes care of that */
    return 0;
}