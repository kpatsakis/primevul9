PackLinuxElf64::elf_has_dynamic(unsigned int key) const
{
    Elf64_Dyn const *dynp= dynseg;
    if (dynp)
    for (; Elf64_Dyn::DT_NULL!=dynp->d_tag; ++dynp) if (get_te64(&dynp->d_tag)==key) {
        return dynp;
    }
    return nullptr;
}