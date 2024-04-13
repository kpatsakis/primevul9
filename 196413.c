nwfilterConnectOpen(virConnectPtr conn,
                    virConnectAuthPtr auth G_GNUC_UNUSED,
                    virConf *conf G_GNUC_UNUSED,
                    unsigned int flags)
{
    virCheckFlags(VIR_CONNECT_RO, VIR_DRV_OPEN_ERROR);

    if (driver == NULL) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("nwfilter state driver is not active"));
        return VIR_DRV_OPEN_ERROR;
    }

    if (STRNEQ(conn->uri->path, "/system")) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("unexpected nwfilter URI path '%s', try nwfilter:///system"),
                       conn->uri->path);
        return VIR_DRV_OPEN_ERROR;
    }

    if (virConnectOpenEnsureACL(conn) < 0)
        return VIR_DRV_OPEN_ERROR;

    return VIR_DRV_OPEN_SUCCESS;
}