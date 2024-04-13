void qemu_ram_remap(ram_addr_t addr, ram_addr_t length)
{
    RAMBlock *block;
    ram_addr_t offset;
    int flags;
    void *area, *vaddr;

    QLIST_FOREACH_RCU(block, &ram_list.blocks, next) {
        offset = addr - block->offset;
        if (offset < block->max_length) {
            vaddr = ramblock_ptr(block, offset);
            if (block->flags & RAM_PREALLOC) {
                ;
            } else if (xen_enabled()) {
                abort();
            } else {
                flags = MAP_FIXED;
                if (block->fd >= 0) {
                    flags |= (block->flags & RAM_SHARED ?
                              MAP_SHARED : MAP_PRIVATE);
                    area = mmap(vaddr, length, PROT_READ | PROT_WRITE,
                                flags, block->fd, offset);
                } else {
                    /*
                     * Remap needs to match alloc.  Accelerators that
                     * set phys_mem_alloc never remap.  If they did,
                     * we'd need a remap hook here.
                     */
                    assert(phys_mem_alloc == qemu_anon_ram_alloc);

                    flags |= MAP_PRIVATE | MAP_ANONYMOUS;
                    area = mmap(vaddr, length, PROT_READ | PROT_WRITE,
                                flags, -1, 0);
                }
                if (area != vaddr) {
                    fprintf(stderr, "Could not remap addr: "
                            RAM_ADDR_FMT "@" RAM_ADDR_FMT "\n",
                            length, addr);
                    exit(1);
                }
                memory_try_enable_merging(vaddr, length);
                qemu_ram_setup_dump(vaddr, length);
            }
        }
    }
}