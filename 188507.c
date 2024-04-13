static int guest_get_network_stats(const char *name,
                                   GuestNetworkInterfaceStat *stats)
{
    OSVERSIONINFO os_ver;

    os_ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&os_ver);
    if (os_ver.dwMajorVersion >= 6) {
        MIB_IF_ROW2 a_mid_ifrow;
        GetIfEntry2Func getifentry2_ex;
        DWORD if_index = 0;
        HMODULE module = GetModuleHandle("iphlpapi");
        PVOID func = GetProcAddress(module, "GetIfEntry2");

        if (func == NULL) {
            return -1;
        }

        getifentry2_ex = (GetIfEntry2Func)func;
        if_index = get_interface_index(name);
        if (if_index == (DWORD)~0) {
            return -1;
        }

        memset(&a_mid_ifrow, 0, sizeof(a_mid_ifrow));
        a_mid_ifrow.InterfaceIndex = if_index;
        if (NO_ERROR == getifentry2_ex(&a_mid_ifrow)) {
            stats->rx_bytes = a_mid_ifrow.InOctets;
            stats->rx_packets = a_mid_ifrow.InUcastPkts;
            stats->rx_errs = a_mid_ifrow.InErrors;
            stats->rx_dropped = a_mid_ifrow.InDiscards;
            stats->tx_bytes = a_mid_ifrow.OutOctets;
            stats->tx_packets = a_mid_ifrow.OutUcastPkts;
            stats->tx_errs = a_mid_ifrow.OutErrors;
            stats->tx_dropped = a_mid_ifrow.OutDiscards;
            return 0;
        }
    }
    return -1;
}