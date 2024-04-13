static void qemu_free_net_client(NetClientState *nc)
{
    if (nc->incoming_queue) {
        qemu_del_net_queue(nc->incoming_queue);
    }
    if (nc->peer) {
        nc->peer->peer = NULL;
    }
    g_free(nc->name);
    g_free(nc->model);
    if (nc->destructor) {
        nc->destructor(nc);
    }
}