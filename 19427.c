virNodeDeviceUpdateCaps(virNodeDeviceDefPtr def)
{
    virNodeDevCapsDefPtr cap = def->caps;

    while (cap) {
        switch (cap->data.type) {
        case VIR_NODE_DEV_CAP_SCSI_HOST:
            virNodeDeviceGetSCSIHostCaps(&cap->data.scsi_host);
            break;
        case VIR_NODE_DEV_CAP_SCSI_TARGET:
            virNodeDeviceGetSCSITargetCaps(def->sysfs_path,
                                           &cap->data.scsi_target);
            break;
        case VIR_NODE_DEV_CAP_NET:
            if (virNetDevGetLinkInfo(cap->data.net.ifname,
                                     &cap->data.net.lnk) < 0)
                return -1;
            virBitmapFree(cap->data.net.features);
            if (virNetDevGetFeatures(cap->data.net.ifname,
                                     &cap->data.net.features) < 0)
                return -1;
            break;
        case VIR_NODE_DEV_CAP_PCI_DEV:
            if (virNodeDeviceGetPCIDynamicCaps(def->sysfs_path,
                                               &cap->data.pci_dev) < 0)
                return -1;
            break;
        case VIR_NODE_DEV_CAP_CSS_DEV:
            if (virNodeDeviceGetCSSDynamicCaps(def->sysfs_path,
                                               &cap->data.ccw_dev) < 0)
                return -1;
            break;

            /* all types that (supposedly) don't require any updates
             * relative to what's in the cache.
             */
        case VIR_NODE_DEV_CAP_DRM:
        case VIR_NODE_DEV_CAP_SYSTEM:
        case VIR_NODE_DEV_CAP_USB_DEV:
        case VIR_NODE_DEV_CAP_USB_INTERFACE:
        case VIR_NODE_DEV_CAP_SCSI:
        case VIR_NODE_DEV_CAP_STORAGE:
        case VIR_NODE_DEV_CAP_FC_HOST:
        case VIR_NODE_DEV_CAP_VPORTS:
        case VIR_NODE_DEV_CAP_SCSI_GENERIC:
        case VIR_NODE_DEV_CAP_MDEV_TYPES:
        case VIR_NODE_DEV_CAP_MDEV:
        case VIR_NODE_DEV_CAP_CCW_DEV:
        case VIR_NODE_DEV_CAP_VDPA:
        case VIR_NODE_DEV_CAP_LAST:
            break;
        }
        cap = cap->next;
    }

    return 0;
}