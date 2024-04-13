ram_addr_t last_ram_offset(void)
{
    RAMBlock *block;
    ram_addr_t last = 0;

    rcu_read_lock();
    QLIST_FOREACH_RCU(block, &ram_list.blocks, next) {
        last = MAX(last, block->offset + block->max_length);
    }
    rcu_read_unlock();
    return last;
}