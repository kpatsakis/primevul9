GuestNetworkInterfaceList *qmp_guest_network_get_interfaces(Error **errp)
{
    IP_ADAPTER_ADDRESSES *adptr_addrs, *addr;
    IP_ADAPTER_UNICAST_ADDRESS *ip_addr = NULL;
    GuestNetworkInterfaceList *head = NULL, *cur_item = NULL;
    GuestIpAddressList *head_addr, *cur_addr;
    GuestNetworkInterfaceList *info;
    GuestNetworkInterfaceStat *interface_stat = NULL;
    GuestIpAddressList *address_item = NULL;
    unsigned char *mac_addr;
    char *addr_str;
    WORD wsa_version;
    WSADATA wsa_data;
    int ret;

    adptr_addrs = guest_get_adapters_addresses(errp);
    if (adptr_addrs == NULL) {
        return NULL;
    }

    /* Make WSA APIs available. */
    wsa_version = MAKEWORD(2, 2);
    ret = WSAStartup(wsa_version, &wsa_data);
    if (ret != 0) {
        error_setg_win32(errp, ret, "failed socket startup");
        goto out;
    }

    for (addr = adptr_addrs; addr; addr = addr->Next) {
        info = g_malloc0(sizeof(*info));

        if (cur_item == NULL) {
            head = cur_item = info;
        } else {
            cur_item->next = info;
            cur_item = info;
        }

        info->value = g_malloc0(sizeof(*info->value));
        info->value->name = guest_wctomb_dup(addr->FriendlyName);

        if (addr->PhysicalAddressLength != 0) {
            mac_addr = addr->PhysicalAddress;

            info->value->hardware_address =
                g_strdup_printf("%02x:%02x:%02x:%02x:%02x:%02x",
                                (int) mac_addr[0], (int) mac_addr[1],
                                (int) mac_addr[2], (int) mac_addr[3],
                                (int) mac_addr[4], (int) mac_addr[5]);

            info->value->has_hardware_address = true;
        }

        head_addr = NULL;
        cur_addr = NULL;
        for (ip_addr = addr->FirstUnicastAddress;
                ip_addr;
                ip_addr = ip_addr->Next) {
            addr_str = guest_addr_to_str(ip_addr, errp);
            if (addr_str == NULL) {
                continue;
            }

            address_item = g_malloc0(sizeof(*address_item));

            if (!cur_addr) {
                head_addr = cur_addr = address_item;
            } else {
                cur_addr->next = address_item;
                cur_addr = address_item;
            }

            address_item->value = g_malloc0(sizeof(*address_item->value));
            address_item->value->ip_address = addr_str;
            address_item->value->prefix = guest_ip_prefix(ip_addr);
            if (ip_addr->Address.lpSockaddr->sa_family == AF_INET) {
                address_item->value->ip_address_type =
                    GUEST_IP_ADDRESS_TYPE_IPV4;
            } else if (ip_addr->Address.lpSockaddr->sa_family == AF_INET6) {
                address_item->value->ip_address_type =
                    GUEST_IP_ADDRESS_TYPE_IPV6;
            }
        }
        if (head_addr) {
            info->value->has_ip_addresses = true;
            info->value->ip_addresses = head_addr;
        }
        if (!info->value->has_statistics) {
            interface_stat = g_malloc0(sizeof(*interface_stat));
            if (guest_get_network_stats(addr->AdapterName,
                interface_stat) == -1) {
                info->value->has_statistics = false;
                g_free(interface_stat);
            } else {
                info->value->statistics = interface_stat;
                info->value->has_statistics = true;
            }
        }
    }
    WSACleanup();
out:
    g_free(adptr_addrs);
    return head;
}