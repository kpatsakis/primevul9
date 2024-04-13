void address_space_unmap(AddressSpace *as, void *buffer, hwaddr len,
                         int is_write, hwaddr access_len)
{
    if (buffer != bounce.buffer) {
        MemoryRegion *mr;
        ram_addr_t addr1;

        mr = qemu_ram_addr_from_host(buffer, &addr1);
        assert(mr != NULL);
        if (is_write) {
            invalidate_and_set_dirty(mr, addr1, access_len);
        }
        if (xen_enabled()) {
            xen_invalidate_map_cache_entry(buffer);
        }
        memory_region_unref(mr);
        return;
    }
    if (is_write) {
        address_space_write(as, bounce.addr, MEMTXATTRS_UNSPECIFIED,
                            bounce.buffer, access_len);
    }
    qemu_vfree(bounce.buffer);
    bounce.buffer = NULL;
    memory_region_unref(bounce.mr);
    atomic_mb_set(&bounce.in_use, false);
    cpu_notify_map_clients();
}