PackNetBSDElf32x86::buildLoader(const Filter *ft)
{
    unsigned char tmp[sizeof(stub_i386_netbsd_elf_fold)];
    memcpy(tmp, stub_i386_netbsd_elf_fold, sizeof(stub_i386_netbsd_elf_fold));
    checkPatch(nullptr, 0, 0, 0);  // reset
    if (opt->o_unix.is_ptinterp) {
        unsigned j;
        for (j = 0; j < sizeof(stub_i386_netbsd_elf_fold)-1; ++j) {
            if (0x60==tmp[  j]
            &&  0x47==tmp[1+j] ) {
                /* put INC EDI before PUSHA: inhibits auxv_up for PT_INTERP */
                tmp[  j] = 0x47;
                tmp[1+j] = 0x60;
                break;
            }
        }
    }
    buildLinuxLoader(
        stub_i386_netbsd_elf_entry, sizeof(stub_i386_netbsd_elf_entry),
        tmp,                        sizeof(stub_i386_netbsd_elf_fold), ft);
}