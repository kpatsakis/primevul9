virNodeDevCapCCWParseXML(xmlXPathContextPtr ctxt,
                         virNodeDeviceDefPtr def,
                         xmlNodePtr node,
                         virNodeDevCapCCWPtr ccw_dev)
{
    VIR_XPATH_NODE_AUTORESTORE(ctxt)
    g_autofree xmlNodePtr *nodes = NULL;
    int n = 0;
    size_t i = 0;
    g_autofree char *cssid = NULL;
    g_autofree char *ssid = NULL;
    g_autofree char *devno = NULL;

    ctxt->node = node;

    if (!(cssid = virXPathString("string(./cssid[1])", ctxt))) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                       _("missing cssid value for '%s'"), def->name);
        return -1;
    }

    if (virStrToLong_uip(cssid, NULL, 0, &ccw_dev->cssid) < 0) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                       _("invalid cssid value '%s' for '%s'"),
                       cssid, def->name);
        return -1;
    }

    if (!(ssid = virXPathString("string(./ssid[1])", ctxt))) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                       _("missing ssid value for '%s'"), def->name);
        return -1;
    }

    if (virStrToLong_uip(ssid, NULL, 0, &ccw_dev->ssid) < 0) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                       _("invalid ssid value '%s' for '%s'"),
                       cssid, def->name);
        return -1;
    }

    if (!(devno = virXPathString("string(./devno[1])", ctxt))) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                       _("missing devno value for '%s'"), def->name);
        return -1;
    }

    if (virStrToLong_uip(devno, NULL, 16, &ccw_dev->devno) < 0) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                       _("invalid devno value '%s' for '%s'"),
                       devno, def->name);
        return -1;
    }

    if ((n = virXPathNodeSet("./capability", ctxt, &nodes)) < 0)
        return -1;

    for (i = 0; i < n; i++) {
        if (virNodeDevCSSCapabilityParseXML(ctxt, nodes[i], ccw_dev) < 0)
            return -1;
    }

    return 0;
}