Elf32_Shdr const *PackLinuxElf32::elf_find_section_type(
    unsigned const type
) const
{
    Elf32_Shdr const *shdr = shdri;
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