PackLinuxElf64::elf_find_dynamic(unsigned int key) const
{
    Elf64_Dyn const *dynp= dynseg;
    if (dynp)
    for (; (unsigned)((char const *)dynp - (char const *)dynseg) < sz_dynseg
            && Elf64_Dyn::DT_NULL!=dynp->d_tag; ++dynp) if (get_te64(&dynp->d_tag)==key) {
        upx_uint64_t const t= elf_get_offset_from_address(get_te64(&dynp->d_val));
        if (t && t < (upx_uint64_t)file_size) {
            return t + file_image;
        }
        break;
    }
    return nullptr;
}