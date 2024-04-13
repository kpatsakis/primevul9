void qemu_using_vnet_hdr(NetClientState *nc, bool enable)
{
    if (!nc || !nc->info->using_vnet_hdr) {
        return;
    }

    nc->info->using_vnet_hdr(nc, enable);
}