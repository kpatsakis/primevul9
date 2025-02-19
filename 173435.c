PackLinuxElf32::elf_find_dynamic(unsigned int key) const
{
    Elf32_Dyn const *dynp= dynseg;
    if (dynp)
    for (; (unsigned)((char const *)dynp - (char const *)dynseg) < sz_dynseg
            && Elf32_Dyn::DT_NULL!=dynp->d_tag; ++dynp) if (get_te32(&dynp->d_tag)==key) {
        unsigned const t= elf_get_offset_from_address(get_te32(&dynp->d_val));
        if (t && t < (unsigned)file_size) {
            return t + file_image;
        }
        break;
    }
    return nullptr;
}