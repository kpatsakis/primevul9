int qemu_set_vnet_be(NetClientState *nc, bool is_be)
{
#ifdef HOST_WORDS_BIGENDIAN
    return 0;
#else
    if (!nc || !nc->info->set_vnet_be) {
        return -ENOSYS;
    }

    return nc->info->set_vnet_be(nc, is_be);
#endif
}