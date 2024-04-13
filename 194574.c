static void qxl_vm_change_state_handler(void *opaque, int running,
                                        RunState state)
{
    PCIQXLDevice *qxl = opaque;

    if (running) {
        /*
         * if qxl_send_events was called from spice server context before
         * migration ended, qxl_update_irq for these events might not have been
         * called
         */
         qxl_update_irq(qxl);
    } else {
        /* make sure surfaces are saved before migration */
        qxl_dirty_surfaces(qxl);
    }
}