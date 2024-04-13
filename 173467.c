PackLinuxElf32mipsel::buildLoader(Filter const *ft)
{
    if (0!=xct_off) {  // shared library
        buildLinuxLoader(
            stub_mipsel_r3000_linux_shlib_init, sizeof(stub_mipsel_r3000_linux_shlib_init),
            nullptr,                        0,                                 ft );
        return;
    }
    buildLinuxLoader(
        stub_mipsel_r3000_linux_elf_entry, sizeof(stub_mipsel_r3000_linux_elf_entry),
        stub_mipsel_r3000_linux_elf_fold,  sizeof(stub_mipsel_r3000_linux_elf_fold), ft);
}