static void qxl_send_events(PCIQXLDevice *d, uint32_t events)
{
    uint32_t old_pending;
    uint32_t le_events = cpu_to_le32(events);

    trace_qxl_send_events(d->id, events);
    if (!qemu_spice_display_is_running(&d->ssd)) {
        /* spice-server tracks guest running state and should not do this */
        fprintf(stderr, "%s: spice-server bug: guest stopped, ignoring\n",
                __func__);
        trace_qxl_send_events_vm_stopped(d->id, events);
        return;
    }
    /*
     * Older versions of Spice forgot to define the QXLRam struct
     * with the '__aligned__(4)' attribute. clang 7 and newer will
     * thus warn that atomic_fetch_or(&d->ram->int_pending, ...)
     * might be a misaligned atomic access, and will generate an
     * out-of-line call for it, which results in a link error since
     * we don't currently link against libatomic.
     *
     * In fact we set up d->ram in init_qxl_ram() so it always starts
     * at a 4K boundary, so we know that &d->ram->int_pending is
     * naturally aligned for a uint32_t. Newer Spice versions
     * (with Spice commit beda5ec7a6848be20c0cac2a9a8ef2a41e8069c1)
     * will fix the bug directly. To deal with older versions,
     * we tell the compiler to assume the address really is aligned.
     * Any compiler which cares about the misalignment will have
     * __builtin_assume_aligned.
     */
#ifdef HAS_ASSUME_ALIGNED
#define ALIGNED_UINT32_PTR(P) ((uint32_t *)__builtin_assume_aligned(P, 4))
#else
#define ALIGNED_UINT32_PTR(P) ((uint32_t *)P)
#endif

    old_pending = atomic_fetch_or(ALIGNED_UINT32_PTR(&d->ram->int_pending),
                                  le_events);
    if ((old_pending & le_events) == le_events) {
        return;
    }
    qemu_bh_schedule(d->update_irq);
}