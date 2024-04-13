static void vnc_init_basic_info(SocketAddress *addr,
                                VncBasicInfo *info,
                                Error **errp)
{
    switch (addr->type) {
    case SOCKET_ADDRESS_TYPE_INET:
        info->host = g_strdup(addr->u.inet.host);
        info->service = g_strdup(addr->u.inet.port);
        if (addr->u.inet.ipv6) {
            info->family = NETWORK_ADDRESS_FAMILY_IPV6;
        } else {
            info->family = NETWORK_ADDRESS_FAMILY_IPV4;
        }
        break;

    case SOCKET_ADDRESS_TYPE_UNIX:
        info->host = g_strdup("");
        info->service = g_strdup(addr->u.q_unix.path);
        info->family = NETWORK_ADDRESS_FAMILY_UNIX;
        break;

    case SOCKET_ADDRESS_TYPE_VSOCK:
    case SOCKET_ADDRESS_TYPE_FD:
        error_setg(errp, "Unsupported socket address type %s",
                   SocketAddressType_lookup[addr->type]);
        break;
    default:
        abort();
    }

    return;
}