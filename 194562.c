static void interface_get_init_info(QXLInstance *sin, QXLDevInitInfo *info)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);

    trace_qxl_interface_get_init_info(qxl->id);
    info->memslot_gen_bits = MEMSLOT_GENERATION_BITS;
    info->memslot_id_bits = MEMSLOT_SLOT_BITS;
    info->num_memslots = NUM_MEMSLOTS;
    info->num_memslots_groups = NUM_MEMSLOTS_GROUPS;
    info->internal_groupslot_id = 0;
    info->qxl_ram_size =
        le32_to_cpu(qxl->shadow_rom.num_pages) << QXL_PAGE_BITS;
    info->n_surfaces = qxl->ssd.num_surfaces;
}