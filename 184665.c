static int memory_access_size(MemoryRegion *mr, unsigned l, hwaddr addr)
{
    unsigned access_size_max = mr->ops->valid.max_access_size;

    /* Regions are assumed to support 1-4 byte accesses unless
       otherwise specified.  */
    if (access_size_max == 0) {
        access_size_max = 4;
    }

    /* Bound the maximum access by the alignment of the address.  */
    if (!mr->ops->impl.unaligned) {
        unsigned align_size_max = addr & -addr;
        if (align_size_max != 0 && align_size_max < access_size_max) {
            access_size_max = align_size_max;
        }
    }

    /* Don't attempt accesses larger than the maximum.  */
    if (l > access_size_max) {
        l = access_size_max;
    }
    if (l & (l - 1)) {
        l = 1 << (qemu_fls(l) - 1);
    }

    return l;
}