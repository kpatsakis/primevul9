PackLinuxElf32armLe::buildLoader(Filter const *ft)
{
    if (Elf32_Ehdr::ELFOSABI_LINUX==ei_osabi) {

        if (0!=xct_off) {  // shared library
            buildLinuxLoader(
                stub_arm_v5t_linux_shlib_init, sizeof(stub_arm_v5t_linux_shlib_init),
                nullptr,                      0,                                ft );
            return;
        }
        buildLinuxLoader(
            stub_arm_v5a_linux_elf_entry, sizeof(stub_arm_v5a_linux_elf_entry),
            stub_arm_v5a_linux_elf_fold,  sizeof(stub_arm_v5a_linux_elf_fold), ft);
    }
    else {
        buildLinuxLoader(
            stub_arm_v4a_linux_elf_entry, sizeof(stub_arm_v4a_linux_elf_entry),
            stub_arm_v4a_linux_elf_fold,  sizeof(stub_arm_v4a_linux_elf_fold), ft);
    }
}