static void interface_set_client_capabilities(QXLInstance *sin,
                                              uint8_t client_present,
                                              uint8_t caps[58])
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);

    if (qxl->revision < 4) {
        trace_qxl_set_client_capabilities_unsupported_by_revision(qxl->id,
                                                              qxl->revision);
        return;
    }

    if (runstate_check(RUN_STATE_INMIGRATE) ||
        runstate_check(RUN_STATE_POSTMIGRATE)) {
        return;
    }

    qxl->shadow_rom.client_present = client_present;
    memcpy(qxl->shadow_rom.client_capabilities, caps,
           sizeof(qxl->shadow_rom.client_capabilities));
    qxl->rom->client_present = client_present;
    memcpy(qxl->rom->client_capabilities, caps,
           sizeof(qxl->rom->client_capabilities));
    qxl_rom_set_dirty(qxl);

    qxl_send_events(qxl, QXL_INTERRUPT_CLIENT);
}