virNodeDeviceCapsListExport(virNodeDeviceDefPtr def,
                            virNodeDevCapType **list)
{
    virNodeDevCapsDefPtr caps = NULL;
    virNodeDevCapType *tmp = NULL;
    bool want_list = !!list;
    int ncaps = 0;
    int ret = -1;

#define MAYBE_ADD_CAP(cap) \
    do { \
        if (want_list) \
            tmp[ncaps] = cap; \
    } while (0)

    if (virNodeDeviceUpdateCaps(def) < 0)
        goto cleanup;

    if (want_list)
        tmp = g_new0(virNodeDevCapType, VIR_NODE_DEV_CAP_LAST - 1);

    for (caps = def->caps; caps; caps = caps->next) {
        unsigned int flags;

        MAYBE_ADD_CAP(caps->data.type);
        ncaps++;

        /* check nested caps for a given type as well */
        if (caps->data.type == VIR_NODE_DEV_CAP_SCSI_HOST) {
            flags = caps->data.scsi_host.flags;

            if (flags & VIR_NODE_DEV_CAP_FLAG_HBA_FC_HOST) {
                MAYBE_ADD_CAP(VIR_NODE_DEV_CAP_FC_HOST);
                ncaps++;
            }

            if (flags  & VIR_NODE_DEV_CAP_FLAG_HBA_VPORT_OPS) {
                MAYBE_ADD_CAP(VIR_NODE_DEV_CAP_VPORTS);
                ncaps++;
            }
        }

        if (caps->data.type == VIR_NODE_DEV_CAP_PCI_DEV) {
            flags = caps->data.pci_dev.flags;

            if (flags & VIR_NODE_DEV_CAP_FLAG_PCI_MDEV) {
                MAYBE_ADD_CAP(VIR_NODE_DEV_CAP_MDEV_TYPES);
                ncaps++;
            }
        }

        if (caps->data.type == VIR_NODE_DEV_CAP_CSS_DEV) {
            flags = caps->data.ccw_dev.flags;

            if (flags & VIR_NODE_DEV_CAP_FLAG_CSS_MDEV) {
                MAYBE_ADD_CAP(VIR_NODE_DEV_CAP_MDEV_TYPES);
                ncaps++;
            }
        }
    }

#undef MAYBE_ADD_CAP

    if (want_list)
        *list = g_steal_pointer(&tmp);
    ret = ncaps;
 cleanup:
    VIR_FREE(tmp);
    return ret;
}