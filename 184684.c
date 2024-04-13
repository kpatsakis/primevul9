int cpu_memory_rw_debug(CPUState *cpu, target_ulong addr,
                        uint8_t *buf, int len, int is_write)
{
    int l;
    hwaddr phys_addr;
    target_ulong page;

    while (len > 0) {
        page = addr & TARGET_PAGE_MASK;
        phys_addr = cpu_get_phys_page_debug(cpu, page);
        /* if no physical page mapped, return an error */
        if (phys_addr == -1)
            return -1;
        l = (page + TARGET_PAGE_SIZE) - addr;
        if (l > len)
            l = len;
        phys_addr += (addr & ~TARGET_PAGE_MASK);
        if (is_write) {
            cpu_physical_memory_write_rom(cpu->as, phys_addr, buf, l);
        } else {
            address_space_rw(cpu->as, phys_addr, MEMTXATTRS_UNSPECIFIED,
                             buf, l, 0);
        }
        len -= l;
        buf += l;
        addr += l;
    }
    return 0;
}