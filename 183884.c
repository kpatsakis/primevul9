static void vnc_init_basic_info_from_server_addr(QIOChannelSocket *ioc,
                                                 VncBasicInfo *info,
                                                 Error **errp)
{
    SocketAddress *addr = NULL;

    if (!ioc) {
        error_setg(errp, "No listener socket available");
        return;
    }

    addr = qio_channel_socket_get_local_address(ioc, errp);
    if (!addr) {
        return;
    }

    vnc_init_basic_info(addr, info, errp);
    qapi_free_SocketAddress(addr);
}