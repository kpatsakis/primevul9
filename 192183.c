void cleanup_insn(insn * i)
{
    free_eops(i->eops);
}