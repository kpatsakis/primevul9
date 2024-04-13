static void breakpoint_invalidate(CPUState *cpu, target_ulong pc)
{
    tb_invalidate_phys_page_range(pc, pc + 1, 0);
}