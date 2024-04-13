void *qemu_get_ram_ptr(ram_addr_t addr)
{
    RAMBlock *block;
    void *ptr;

    rcu_read_lock();
    block = qemu_get_ram_block(addr);

    if (xen_enabled() && block->host == NULL) {
        /* We need to check if the requested address is in the RAM
         * because we don't want to map the entire memory in QEMU.
         * In that case just map until the end of the page.
         */
        if (block->offset == 0) {
            ptr = xen_map_cache(addr, 0, 0);
            goto unlock;
        }

        block->host = xen_map_cache(block->offset, block->max_length, 1);
    }
    ptr = ramblock_ptr(block, addr - block->offset);

unlock:
    rcu_read_unlock();
    return ptr;
}