virNodeDevCSSCapabilityParseXML(xmlXPathContextPtr ctxt,
                                xmlNodePtr node,
                                virNodeDevCapCCWPtr ccw_dev)
{
    g_autofree char *type = virXMLPropString(node, "type");
    VIR_XPATH_NODE_AUTORESTORE(ctxt)

    ctxt->node = node;

    if (!type) {
        virReportError(VIR_ERR_XML_ERROR, "%s", _("Missing capability type"));
        return -1;
    }

    if (STREQ(type, "mdev_types")) {
        if (virNodeDevCapMdevTypesParseXML(ctxt,
                                           &ccw_dev->mdev_types,
                                           &ccw_dev->nmdev_types) < 0)
            return -1;
        ccw_dev->flags |= VIR_NODE_DEV_CAP_FLAG_CSS_MDEV;
    }

    return 0;
}