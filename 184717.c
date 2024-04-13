static void *qemu_ram_ptr_length(ram_addr_t addr, hwaddr *size)
{
    void *ptr;
    if (*size == 0) {
        return NULL;
    }
    if (xen_enabled()) {
        return xen_map_cache(addr, *size, 1);
    } else {
        RAMBlock *block;
        rcu_read_lock();
        QLIST_FOREACH_RCU(block, &ram_list.blocks, next) {
            if (addr - block->offset < block->max_length) {
                if (addr - block->offset + *size > block->max_length)
                    *size = block->max_length - addr + block->offset;
                ptr = ramblock_ptr(block, addr - block->offset);
                rcu_read_unlock();
                return ptr;
            }
        }

        fprintf(stderr, "Bad ram offset %" PRIx64 "\n", (uint64_t)addr);
        abort();
    }
}