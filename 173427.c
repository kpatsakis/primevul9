Elf64_Shdr const *PackLinuxElf64::elf_find_section_name(
    char const *const name
) const
{
    Elf64_Shdr const *shdr = shdri;
    if (!shdr) {
        return nullptr;
    }
    int j = e_shnum;
    for (; 0 <=--j; ++shdr) {
        unsigned const sh_name = get_te32(&shdr->sh_name);
        if ((u32_t)file_size <= sh_name) {  // FIXME: weak
            char msg[50]; snprintf(msg, sizeof(msg),
                "bad Elf64_Shdr[%d].sh_name %#x",
                -1+ e_shnum -j, sh_name);
            throwCantPack(msg);
        }
        if (0==strcmp(name, &shstrtab[sh_name])) {
            return shdr;
        }
    }
    return nullptr;
}