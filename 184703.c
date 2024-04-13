ram_addr_t qemu_ram_alloc_from_file(ram_addr_t size, MemoryRegion *mr,
                                    bool share, const char *mem_path,
                                    Error **errp)
{
    RAMBlock *new_block;
    ram_addr_t addr;
    Error *local_err = NULL;

    if (xen_enabled()) {
        error_setg(errp, "-mem-path not supported with Xen");
        return -1;
    }

    if (phys_mem_alloc != qemu_anon_ram_alloc) {
        /*
         * file_ram_alloc() needs to allocate just like
         * phys_mem_alloc, but we haven't bothered to provide
         * a hook there.
         */
        error_setg(errp,
                   "-mem-path not supported with this accelerator");
        return -1;
    }

    size = TARGET_PAGE_ALIGN(size);
    new_block = g_malloc0(sizeof(*new_block));
    new_block->mr = mr;
    new_block->used_length = size;
    new_block->max_length = size;
    new_block->flags = share ? RAM_SHARED : 0;
    new_block->host = file_ram_alloc(new_block, size,
                                     mem_path, errp);
    if (!new_block->host) {
        g_free(new_block);
        return -1;
    }

    addr = ram_block_add(new_block, &local_err);
    if (local_err) {
        g_free(new_block);
        error_propagate(errp, local_err);
        return -1;
    }
    return addr;
}