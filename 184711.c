int qemu_ram_resize(ram_addr_t base, ram_addr_t newsize, Error **errp)
{
    RAMBlock *block = find_ram_block(base);

    assert(block);

    newsize = TARGET_PAGE_ALIGN(newsize);

    if (block->used_length == newsize) {
        return 0;
    }

    if (!(block->flags & RAM_RESIZEABLE)) {
        error_setg_errno(errp, EINVAL,
                         "Length mismatch: %s: 0x" RAM_ADDR_FMT
                         " in != 0x" RAM_ADDR_FMT, block->idstr,
                         newsize, block->used_length);
        return -EINVAL;
    }

    if (block->max_length < newsize) {
        error_setg_errno(errp, EINVAL,
                         "Length too large: %s: 0x" RAM_ADDR_FMT
                         " > 0x" RAM_ADDR_FMT, block->idstr,
                         newsize, block->max_length);
        return -EINVAL;
    }

    cpu_physical_memory_clear_dirty_range(block->offset, block->used_length);
    block->used_length = newsize;
    cpu_physical_memory_set_dirty_range(block->offset, block->used_length,
                                        DIRTY_CLIENTS_ALL);
    memory_region_set_size(block->mr, newsize);
    if (block->resized) {
        block->resized(block->idstr, newsize, block->host);
    }
    return 0;
}