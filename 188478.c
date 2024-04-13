static int64_t guest_ip_prefix(IP_ADAPTER_UNICAST_ADDRESS *ip_addr)
{
    int64_t prefix = -1; /* Use for AF_INET6 and unknown/undetermined values. */
    IP_ADAPTER_INFO *adptr_info, *info;
    IP_ADDR_STRING *ip;
    struct in_addr *p;

    if (ip_addr->Address.lpSockaddr->sa_family != AF_INET) {
        return prefix;
    }
    adptr_info = guest_get_adapters_info();
    if (adptr_info == NULL) {
        return prefix;
    }

    /* Match up the passed in ip_addr with one found in adaptr_info.
     * The matching one in adptr_info will have the netmask.
     */
    p = &((struct sockaddr_in *)ip_addr->Address.lpSockaddr)->sin_addr;
    for (info = adptr_info; info; info = info->Next) {
        for (ip = &info->IpAddressList; ip; ip = ip->Next) {
            if (p->S_un.S_addr == inet_addr(ip->IpAddress.String)) {
                prefix = ctpop32(inet_addr(ip->IpMask.String));
                goto out;
            }
        }
    }
out:
    g_free(adptr_info);
    return prefix;
}