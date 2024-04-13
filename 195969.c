void qemu_set_vnet_hdr_len(NetClientState *nc, int len)
{
    if (!nc || !nc->info->set_vnet_hdr_len) {
        return;
    }

    nc->vnet_hdr_len = len;
    nc->info->set_vnet_hdr_len(nc, len);
}