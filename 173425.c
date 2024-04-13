PackLinuxElf32::getbase(const Elf32_Phdr *phdr, int nph) const
{
    off_t base = ~0u;
    for (int j = 0; j < nph; ++phdr, ++j) {
        if (is_LOAD32(phdr)) {
            unsigned const vaddr = get_te32(&phdr->p_vaddr);
            if (vaddr < (unsigned) base)
                base = vaddr;
        }
    }
    if (0!=base) {
        return base;
    }
    return 0x12000;
}