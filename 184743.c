static RAMBlock *qemu_get_ram_block(ram_addr_t addr)
{
    RAMBlock *block;

    block = atomic_rcu_read(&ram_list.mru_block);
    if (block && addr - block->offset < block->max_length) {
        goto found;
    }
    QLIST_FOREACH_RCU(block, &ram_list.blocks, next) {
        if (addr - block->offset < block->max_length) {
            goto found;
        }
    }

    fprintf(stderr, "Bad ram offset %" PRIx64 "\n", (uint64_t)addr);
    abort();

found:
    /* It is safe to write mru_block outside the iothread lock.  This
     * is what happens:
     *
     *     mru_block = xxx
     *     rcu_read_unlock()
     *                                        xxx removed from list
     *                  rcu_read_lock()
     *                  read mru_block
     *                                        mru_block = NULL;
     *                                        call_rcu(reclaim_ramblock, xxx);
     *                  rcu_read_unlock()
     *
     * atomic_rcu_set is not needed here.  The block was already published
     * when it was placed into the list.  Here we're just making an extra
     * copy of the pointer.
     */
    ram_list.mru_block = block;
    return block;
}