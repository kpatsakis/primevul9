MemoryRegion *qemu_ram_addr_from_host(void *ptr, ram_addr_t *ram_addr)
{
    RAMBlock *block;
    uint8_t *host = ptr;
    MemoryRegion *mr;

    if (xen_enabled()) {
        rcu_read_lock();
        *ram_addr = xen_ram_addr_from_mapcache(ptr);
        mr = qemu_get_ram_block(*ram_addr)->mr;
        rcu_read_unlock();
        return mr;
    }

    rcu_read_lock();
    block = atomic_rcu_read(&ram_list.mru_block);
    if (block && block->host && host - block->host < block->max_length) {
        goto found;
    }

    QLIST_FOREACH_RCU(block, &ram_list.blocks, next) {
        /* This case append when the block is not mapped. */
        if (block->host == NULL) {
            continue;
        }
        if (host - block->host < block->max_length) {
            goto found;
        }
    }

    rcu_read_unlock();
    return NULL;

found:
    *ram_addr = block->offset + (host - block->host);
    mr = block->mr;
    rcu_read_unlock();
    return mr;
}