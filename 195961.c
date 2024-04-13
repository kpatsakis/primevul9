int qemu_set_vnet_le(NetClientState *nc, bool is_le)
{
#ifdef HOST_WORDS_BIGENDIAN
    if (!nc || !nc->info->set_vnet_le) {
        return -ENOSYS;
    }

    return nc->info->set_vnet_le(nc, is_le);
#else
    return 0;
#endif
}