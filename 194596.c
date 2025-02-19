static void interface_release_resource(QXLInstance *sin,
                                       QXLReleaseInfoExt ext)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);
    QXLReleaseRing *ring;
    uint64_t *item, id;

    if (!ext.info) {
        return;
    }
    if (ext.group_id == MEMSLOT_GROUP_HOST) {
        /* host group -> vga mode update request */
        QXLCommandExt *cmdext = (void *)(intptr_t)(ext.info->id);
        SimpleSpiceUpdate *update;
        g_assert(cmdext->cmd.type == QXL_CMD_DRAW);
        update = container_of(cmdext, SimpleSpiceUpdate, ext);
        qemu_spice_destroy_update(&qxl->ssd, update);
        return;
    }

    /*
     * ext->info points into guest-visible memory
     * pci bar 0, $command.release_info
     */
    ring = &qxl->ram->release_ring;
    SPICE_RING_PROD_ITEM(qxl, ring, item);
    if (!item) {
        return;
    }
    if (*item == 0) {
        /* stick head into the ring */
        id = ext.info->id;
        ext.info->next = 0;
        qxl_ram_set_dirty(qxl, &ext.info->next);
        *item = id;
        qxl_ring_set_dirty(qxl);
    } else {
        /* append item to the list */
        qxl->last_release->next = ext.info->id;
        qxl_ram_set_dirty(qxl, &qxl->last_release->next);
        ext.info->next = 0;
        qxl_ram_set_dirty(qxl, &ext.info->next);
    }
    qxl->last_release = ext.info;
    qxl->num_free_res++;
    trace_qxl_ring_res_put(qxl->id, qxl->num_free_res);
    qxl_push_free_res(qxl, 0);
}