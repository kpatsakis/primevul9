PackLinuxElf32::getbrk(Elf32_Phdr const *phdr, int nph) const
{
    off_t brka = 0;
    for (int j = 0; j < nph; ++phdr, ++j) {
        if (is_LOAD32(phdr)) {
            off_t b = get_te32(&phdr->p_vaddr) + get_te32(&phdr->p_memsz);
            if (b > brka)
                brka = b;
        }
    }
    return brka;
}