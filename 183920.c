static void vnc_display_print_local_addr(VncDisplay *vd)
{
    SocketAddress *addr;
    Error *err = NULL;

    if (!vd->nlsock) {
        return;
    }

    addr = qio_channel_socket_get_local_address(vd->lsock[0], &err);
    if (!addr) {
        return;
    }

    if (addr->type != SOCKET_ADDRESS_TYPE_INET) {
        qapi_free_SocketAddress(addr);
        return;
    }
    error_printf_unless_qmp("VNC server running on %s:%s\n",
                            addr->u.inet.host,
                            addr->u.inet.port);
    qapi_free_SocketAddress(addr);
}