bool qemu_has_ufo(NetClientState *nc)
{
    if (!nc || !nc->info->has_ufo) {
        return false;
    }

    return nc->info->has_ufo(nc);
}