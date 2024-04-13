void qemu_set_offload(NetClientState *nc, int csum, int tso4, int tso6,
                          int ecn, int ufo)
{
    if (!nc || !nc->info->set_offload) {
        return;
    }

    nc->info->set_offload(nc, csum, tso4, tso6, ecn, ufo);
}