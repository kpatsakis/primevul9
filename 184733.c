bool cpu_physical_memory_test_and_clear_dirty(ram_addr_t start,
                                              ram_addr_t length,
                                              unsigned client)
{
    unsigned long end, page;
    bool dirty;

    if (length == 0) {
        return false;
    }

    end = TARGET_PAGE_ALIGN(start + length) >> TARGET_PAGE_BITS;
    page = start >> TARGET_PAGE_BITS;
    dirty = bitmap_test_and_clear_atomic(ram_list.dirty_memory[client],
                                         page, end - page);

    if (dirty && tcg_enabled()) {
        tlb_reset_dirty_range_all(start, length);
    }

    return dirty;
}