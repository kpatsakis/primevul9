PackLinuxElf32::elf_has_dynamic(unsigned int key) const
{
    Elf32_Dyn const *dynp= dynseg;
    if (dynp)
    for (; Elf32_Dyn::DT_NULL!=dynp->d_tag; ++dynp) if (get_te32(&dynp->d_tag)==key) {
        return dynp;
    }
    return nullptr;
}