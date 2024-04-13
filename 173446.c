Elf64_Shdr const *PackLinuxElf64::elf_find_section_type(
    unsigned const type
) const
{
    Elf64_Shdr const *shdr = shdri;
    if (!shdr) {
        return nullptr;
    }
    int j = e_shnum;
    for (; 0 <=--j; ++shdr) {
        if (type==get_te32(&shdr->sh_type)) {
            return shdr;
        }
    }
    return nullptr;
}