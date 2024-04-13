bool qemu_has_vnet_hdr(NetClientState *nc)
{
    if (!nc || !nc->info->has_vnet_hdr) {
        return false;
    }

    return nc->info->has_vnet_hdr(nc);
}