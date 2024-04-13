virNodeDevCapMdevTypesParseXML(xmlXPathContextPtr ctxt,
                               virMediatedDeviceTypePtr **mdev_types,
                               size_t *nmdev_types)
{
    int ret = -1;
    xmlNodePtr orignode = NULL;
    xmlNodePtr *nodes = NULL;
    int ntypes = -1;
    virMediatedDeviceTypePtr type = NULL;
    size_t i;

    if ((ntypes = virXPathNodeSet("./type", ctxt, &nodes)) < 0)
        goto cleanup;

    if (nmdev_types == 0) {
        virReportError(VIR_ERR_XML_ERROR, "%s",
                       _("missing <type> element in <capability> element"));
        goto cleanup;
    }

    orignode = ctxt->node;
    for (i = 0; i < ntypes; i++) {
        ctxt->node = nodes[i];

        type = g_new0(virMediatedDeviceType, 1);

        if (!(type->id = virXPathString("string(./@id[1])", ctxt))) {
            virReportError(VIR_ERR_XML_ERROR, "%s",
                           _("missing 'id' attribute for mediated device's "
                             "<type> element"));
            goto cleanup;
        }

        if (!(type->device_api = virXPathString("string(./deviceAPI[1])", ctxt))) {
            virReportError(VIR_ERR_XML_ERROR,
                           _("missing device API for mediated device type '%s'"),
                           type->id);
            goto cleanup;
        }

        if (virXPathUInt("number(./availableInstances)", ctxt,
                         &type->available_instances) < 0) {
            virReportError(VIR_ERR_XML_ERROR,
                           _("missing number of available instances for "
                             "mediated device type '%s'"),
                           type->id);
            goto cleanup;
        }

        type->name = virXPathString("string(./name)", ctxt);

        if (VIR_APPEND_ELEMENT(*mdev_types,
                               *nmdev_types, type) < 0)
            goto cleanup;
    }

    ret = 0;
 cleanup:
    VIR_FREE(nodes);
    virMediatedDeviceTypeFree(type);
    ctxt->node = orignode;
    return ret;
}