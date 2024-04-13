virNodeDevPCICapabilityParseXML(xmlXPathContextPtr ctxt,
                                xmlNodePtr node,
                                virNodeDevCapPCIDevPtr pci_dev)
{
    char *type = virXMLPropString(node, "type");
    VIR_XPATH_NODE_AUTORESTORE(ctxt)
    int ret = -1;

    ctxt->node = node;

    if (!type) {
        virReportError(VIR_ERR_XML_ERROR, "%s", _("Missing capability type"));
        goto cleanup;
    }

    if (STREQ(type, "phys_function") &&
        virNodeDevPCICapSRIOVPhysicalParseXML(ctxt, pci_dev) < 0) {
        goto cleanup;
    } else if (STREQ(type, "virt_functions") &&
               virNodeDevPCICapSRIOVVirtualParseXML(ctxt, pci_dev) < 0) {
        goto cleanup;
    } else if (STREQ(type, "mdev_types")) {
        if (virNodeDevCapMdevTypesParseXML(ctxt,
                                           &pci_dev->mdev_types,
                                           &pci_dev->nmdev_types) < 0)
            goto cleanup;
        pci_dev->flags |= VIR_NODE_DEV_CAP_FLAG_PCI_MDEV;
    } else {
        int hdrType = virPCIHeaderTypeFromString(type);

        if (hdrType > 0 && !pci_dev->hdrType)
            pci_dev->hdrType = hdrType;
    }

    ret = 0;
 cleanup:
    VIR_FREE(type);
    return ret;
}