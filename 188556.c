static char *guest_addr_to_str(IP_ADAPTER_UNICAST_ADDRESS *ip_addr,
                               Error **errp)
{
    char addr_str[INET6_ADDRSTRLEN + INET_ADDRSTRLEN];
    DWORD len;
    int ret;

    if (ip_addr->Address.lpSockaddr->sa_family == AF_INET ||
            ip_addr->Address.lpSockaddr->sa_family == AF_INET6) {
        len = sizeof(addr_str);
        ret = WSAAddressToString(ip_addr->Address.lpSockaddr,
                                 ip_addr->Address.iSockaddrLength,
                                 NULL,
                                 addr_str,
                                 &len);
        if (ret != 0) {
            error_setg_win32(errp, WSAGetLastError(),
                "failed address presentation form conversion");
            return NULL;
        }
        return g_strdup(addr_str);
    }
    return NULL;
}