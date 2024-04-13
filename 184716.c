void qemu_ram_unset_idstr(ram_addr_t addr)
{
    RAMBlock *block;

    /* FIXME: arch_init.c assumes that this is not called throughout
     * migration.  Ignore the problem since hot-unplug during migration
     * does not work anyway.
     */

    rcu_read_lock();
    block = find_ram_block(addr);
    if (block) {
        memset(block->idstr, 0, sizeof(block->idstr));
    }
    rcu_read_unlock();
}