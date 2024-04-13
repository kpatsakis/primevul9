static MemTxResult watch_mem_write(void *opaque, hwaddr addr,
                                   uint64_t val, unsigned size,
                                   MemTxAttrs attrs)
{
    MemTxResult res;

    check_watchpoint(addr & ~TARGET_PAGE_MASK, size, attrs, BP_MEM_WRITE);
    switch (size) {
    case 1:
        address_space_stb(&address_space_memory, addr, val, attrs, &res);
        break;
    case 2:
        address_space_stw(&address_space_memory, addr, val, attrs, &res);
        break;
    case 4:
        address_space_stl(&address_space_memory, addr, val, attrs, &res);
        break;
    default: abort();
    }
    return res;
}