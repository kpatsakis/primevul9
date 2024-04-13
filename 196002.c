static void qemu_net_client_destructor(NetClientState *nc)
{
    g_free(nc);
}