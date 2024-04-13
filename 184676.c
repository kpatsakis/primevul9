static void breakpoint_invalidate(CPUState *cpu, target_ulong pc)
{
    hwaddr phys = cpu_get_phys_page_debug(cpu, pc);
    if (phys != -1) {
        tb_invalidate_phys_addr(cpu->as,
                                phys | (pc & ~TARGET_PAGE_MASK));
    }
}