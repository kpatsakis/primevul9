void qemu_format_nic_info_str(NetClientState *nc, uint8_t macaddr[6])
{
    snprintf(nc->info_str, sizeof(nc->info_str),
             "model=%s,macaddr=%02x:%02x:%02x:%02x:%02x:%02x",
             nc->model,
             macaddr[0], macaddr[1], macaddr[2],
             macaddr[3], macaddr[4], macaddr[5]);
}