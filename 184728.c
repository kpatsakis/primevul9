int qemu_get_ram_fd(ram_addr_t addr)
{
    RAMBlock *block;
    int fd;

    rcu_read_lock();
    block = qemu_get_ram_block(addr);
    fd = block->fd;
    rcu_read_unlock();
    return fd;
}