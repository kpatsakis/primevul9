PackLinuxElf64amd::buildLoader(const Filter *ft)
{
    if (0!=xct_off) {  // shared library
        buildLinuxLoader(
            stub_amd64_linux_shlib_init, sizeof(stub_amd64_linux_shlib_init),
            nullptr,                        0,                                 ft );
        return;
    }
    buildLinuxLoader(
        stub_amd64_linux_elf_entry, sizeof(stub_amd64_linux_elf_entry),
        stub_amd64_linux_elf_fold,  sizeof(stub_amd64_linux_elf_fold), ft);
}