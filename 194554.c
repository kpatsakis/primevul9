static void qxl_spice_monitors_config_async(PCIQXLDevice *qxl, int replay)
{
    QXLMonitorsConfig *cfg;

    trace_qxl_spice_monitors_config(qxl->id);
    if (replay) {
        /*
         * don't use QXL_COOKIE_TYPE_IO:
         *  - we are not running yet (post_load), we will assert
         *    in send_events
         *  - this is not a guest io, but a reply, so async_io isn't set.
         */
        spice_qxl_monitors_config_async(&qxl->ssd.qxl,
                qxl->guest_monitors_config,
                MEMSLOT_GROUP_GUEST,
                (uintptr_t)qxl_cookie_new(
                    QXL_COOKIE_TYPE_POST_LOAD_MONITORS_CONFIG,
                    0));
    } else {
/* >= release 0.12.6, < release 0.14.2 */
#if SPICE_SERVER_VERSION >= 0x000c06 && SPICE_SERVER_VERSION < 0x000e02
        if (qxl->max_outputs) {
            spice_qxl_set_max_monitors(&qxl->ssd.qxl, qxl->max_outputs);
        }
#endif
        qxl->guest_monitors_config = qxl->ram->monitors_config;
        spice_qxl_monitors_config_async(&qxl->ssd.qxl,
                qxl->ram->monitors_config,
                MEMSLOT_GROUP_GUEST,
                (uintptr_t)qxl_cookie_new(QXL_COOKIE_TYPE_IO,
                                          QXL_IO_MONITORS_CONFIG_ASYNC));
    }

    cfg = qxl_phys2virt(qxl, qxl->guest_monitors_config, MEMSLOT_GROUP_GUEST);
    if (cfg != NULL && cfg->count == 1) {
        qxl->guest_primary.resized = 1;
        qxl->guest_head0_width  = cfg->heads[0].width;
        qxl->guest_head0_height = cfg->heads[0].height;
    } else {
        qxl->guest_head0_width  = 0;
        qxl->guest_head0_height = 0;
    }
}