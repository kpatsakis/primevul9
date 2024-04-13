PackLinuxElf32::elf_get_offset_from_address(unsigned addr) const
{
    Elf32_Phdr const *phdr = phdri;
    int j = e_phnum;
    for (; --j>=0; ++phdr) if (is_LOAD32(phdr)) {
        unsigned const t = addr - get_te32(&phdr->p_vaddr);
        if (t < get_te32(&phdr->p_filesz)) {
            unsigned const p_offset = get_te32(&phdr->p_offset);
            if ((u32_t)file_size <= p_offset) { // FIXME: weak
                char msg[40]; snprintf(msg, sizeof(msg),
                    "bad Elf32_Phdr[%d].p_offset %x",
                    -1+ e_phnum - j, p_offset);
                throwCantPack(msg);
            }
            return t + p_offset;
        }
    }
    return 0;
}