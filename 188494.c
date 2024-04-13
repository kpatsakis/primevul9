static int guest_get_network_stats(const char *name,
                       GuestNetworkInterfaceStat *stats)
{
    int name_len;
    char const *devinfo = "/proc/net/dev";
    FILE *fp;
    char *line = NULL, *colon;
    size_t n = 0;
    fp = fopen(devinfo, "r");
    if (!fp) {
        return -1;
    }
    name_len = strlen(name);
    while (getline(&line, &n, fp) != -1) {
        long long dummy;
        long long rx_bytes;
        long long rx_packets;
        long long rx_errs;
        long long rx_dropped;
        long long tx_bytes;
        long long tx_packets;
        long long tx_errs;
        long long tx_dropped;
        char *trim_line;
        trim_line = g_strchug(line);
        if (trim_line[0] == '\0') {
            continue;
        }
        colon = strchr(trim_line, ':');
        if (!colon) {
            continue;
        }
        if (colon - name_len  == trim_line &&
           strncmp(trim_line, name, name_len) == 0) {
            if (sscanf(colon + 1,
                "%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
                  &rx_bytes, &rx_packets, &rx_errs, &rx_dropped,
                  &dummy, &dummy, &dummy, &dummy,
                  &tx_bytes, &tx_packets, &tx_errs, &tx_dropped,
                  &dummy, &dummy, &dummy, &dummy) != 16) {
                continue;
            }
            stats->rx_bytes = rx_bytes;
            stats->rx_packets = rx_packets;
            stats->rx_errs = rx_errs;
            stats->rx_dropped = rx_dropped;
            stats->tx_bytes = tx_bytes;
            stats->tx_packets = tx_packets;
            stats->tx_errs = tx_errs;
            stats->tx_dropped = tx_dropped;
            fclose(fp);
            g_free(line);
            return 0;
        }
    }
    fclose(fp);
    g_free(line);
    g_debug("/proc/net/dev: Interface '%s' not found", name);
    return -1;
}