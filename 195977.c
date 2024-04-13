bool qemu_has_vnet_hdr_len(NetClientState *nc, int len)
{
    if (!nc || !nc->info->has_vnet_hdr_len) {
        return false;
    }

    return nc->info->has_vnet_hdr_len(nc, len);
}