static void qxl_hard_reset(PCIQXLDevice *d, int loadvm)
{
    bool startstop = qemu_spice_display_is_running(&d->ssd);

    trace_qxl_hard_reset(d->id, loadvm);

    if (startstop) {
        qemu_spice_display_stop();
    }

    qxl_spice_reset_cursor(d);
    qxl_spice_reset_image_cache(d);
    qxl_reset_surfaces(d);
    qxl_reset_memslots(d);

    /* pre loadvm reset must not touch QXLRam.  This lives in
     * device memory, is migrated together with RAM and thus
     * already loaded at this point */
    if (!loadvm) {
        qxl_reset_state(d);
    }
    qemu_spice_create_host_memslot(&d->ssd);
    qxl_soft_reset(d);

    if (d->migration_blocker) {
        migrate_del_blocker(d->migration_blocker);
        error_free(d->migration_blocker);
        d->migration_blocker = NULL;
    }

    if (startstop) {
        qemu_spice_display_start();
    }
}