int PackLinuxElf32::is_LOAD32(Elf32_Phdr const *phdr) const
{
    // (1+ PT_LOPROC) can confuse!
    return PT_LOAD32 == get_te32(&phdr->p_type);
}