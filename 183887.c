VncInfo *qmp_query_vnc(Error **errp)
{
    VncInfo *info = g_malloc0(sizeof(*info));
    VncDisplay *vd = vnc_display_find(NULL);
    SocketAddress *addr = NULL;

    if (vd == NULL || !vd->nlsock) {
        info->enabled = false;
    } else {
        info->enabled = true;

        /* for compatibility with the original command */
        info->has_clients = true;
        info->clients = qmp_query_client_list(vd);

        if (vd->lsock == NULL) {
            return info;
        }

        addr = qio_channel_socket_get_local_address(vd->lsock[0], errp);
        if (!addr) {
            goto out_error;
        }

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
            goto out_error;
        default:
            abort();
        }

        info->has_host = true;
        info->has_service = true;
        info->has_family = true;

        info->has_auth = true;
        info->auth = g_strdup(vnc_auth_name(vd));
    }

    qapi_free_SocketAddress(addr);
    return info;

out_error:
    qapi_free_SocketAddress(addr);
    qapi_free_VncInfo(info);
    return NULL;
}