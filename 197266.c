void qemu_flush_or_purge_queued_packets(NetClientState *nc, bool purge)
{
    nc->receive_disabled = 0;

    if (nc->peer && nc->peer->info->type == NET_CLIENT_DRIVER_HUBPORT) {
        if (net_hub_flush(nc->peer)) {
            qemu_notify_event();
        }
    }
    if (qemu_net_queue_flush(nc->incoming_queue)) {
        /* We emptied the queue successfully, signal to the IO thread to repoll
         * the file descriptor (for tap, for example).
         */
        qemu_notify_event();
    } else if (purge) {
        /* Unable to empty the queue, purge remaining packets */
        qemu_net_queue_purge(nc->incoming_queue, nc);
    }
}