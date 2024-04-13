void *qemu_get_ram_block_host_ptr(ram_addr_t addr)
{
    RAMBlock *block;
    void *ptr;

    rcu_read_lock();
    block = qemu_get_ram_block(addr);
    ptr = ramblock_ptr(block, 0);
    rcu_read_unlock();
    return ptr;
}