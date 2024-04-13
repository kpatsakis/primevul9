static void tlb_reset_dirty_range_all(ram_addr_t start, ram_addr_t length)
{
    ram_addr_t start1;
    RAMBlock *block;
    ram_addr_t end;

    end = TARGET_PAGE_ALIGN(start + length);
    start &= TARGET_PAGE_MASK;

    rcu_read_lock();
    block = qemu_get_ram_block(start);
    assert(block == qemu_get_ram_block(end - 1));
    start1 = (uintptr_t)ramblock_ptr(block, start - block->offset);
    cpu_tlb_reset_dirty_all(start1, length);
    rcu_read_unlock();
}