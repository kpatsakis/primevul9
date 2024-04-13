static void net_vm_change_state_handler(void *opaque, bool running,
                                        RunState state)
{
    NetClientState *nc;
    NetClientState *tmp;

    QTAILQ_FOREACH_SAFE(nc, &net_clients, next, tmp) {
        if (running) {
            /* Flush queued packets and wake up backends. */
            if (nc->peer && qemu_can_send_packet(nc)) {
                qemu_flush_queued_packets(nc->peer);
            }
        } else {
            /* Complete all queued packets, to guarantee we don't modify
             * state later when VM is not running.
             */
            qemu_flush_or_purge_queued_packets(nc, true);
        }
    }
}