void qemu_ram_free(ram_addr_t addr)
{
    RAMBlock *block;

    qemu_mutex_lock_ramlist();
    QLIST_FOREACH_RCU(block, &ram_list.blocks, next) {
        if (addr == block->offset) {
            QLIST_REMOVE_RCU(block, next);
            ram_list.mru_block = NULL;
            /* Write list before version */
            smp_wmb();
            ram_list.version++;
            call_rcu(block, reclaim_ramblock, rcu);
            break;
        }
    }
    qemu_mutex_unlock_ramlist();
}