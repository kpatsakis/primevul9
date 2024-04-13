PackLinuxElf64::elf_find_ptype(unsigned type, Elf64_Phdr const *phdr, unsigned phnum)
{
    for (unsigned j = 0; j < phnum; ++j, ++phdr) {
        if (type == get_te32(&phdr->p_type)) {
            return phdr;
        }
    }
    return nullptr;
}