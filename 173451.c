PackLinuxElf64::generateElfHdr(
    OutputFile *fo,
    void const *proto,
    unsigned const brka
)
{
    cprElfHdr2 *const h2 = (cprElfHdr2 *)(void *)&elfout;
    cprElfHdr3 *const h3 = (cprElfHdr3 *)(void *)&elfout;
    h3->ehdr =         ((cprElfHdr3 const *)proto)->ehdr;
    h3->phdr[C_BASE] = ((cprElfHdr3 const *)proto)->phdr[1];  // .data; .p_align
    h3->phdr[C_TEXT] = ((cprElfHdr3 const *)proto)->phdr[0];  // .text
    memset(&h3->linfo, 0, sizeof(h3->linfo));

    h3->ehdr.e_type = ehdri.e_type;  // ET_EXEC vs ET_DYN (gcc -pie -fPIC)
    h3->ehdr.e_ident[Elf64_Ehdr::EI_OSABI] = ei_osabi;
    if (Elf64_Ehdr::ELFOSABI_LINUX == ei_osabi  // proper
    &&  Elf64_Ehdr::ELFOSABI_NONE  == ehdri.e_ident[Elf64_Ehdr::EI_OSABI]  // sloppy
    ) { // propagate sloppiness so that decompression does not complain
        h3->ehdr.e_ident[Elf64_Ehdr::EI_OSABI] = ehdri.e_ident[Elf64_Ehdr::EI_OSABI];
    }
    if (Elf64_Ehdr::EM_PPC64 == get_te16(&ehdri.e_machine)) {
        h3->ehdr.e_flags = ehdri.e_flags;  // "0x1, abiv1" vs "0x2, abiv2"
    }

    unsigned const phnum_i = get_te16(&h2->ehdr.e_phnum);
    unsigned       phnum_o = phnum_i;

    assert(get_te64(&h2->ehdr.e_phoff)     == sizeof(Elf64_Ehdr));
                         h2->ehdr.e_shoff = 0;
    assert(get_te16(&h2->ehdr.e_ehsize)    == sizeof(Elf64_Ehdr));
    assert(get_te16(&h2->ehdr.e_phentsize) == sizeof(Elf64_Phdr));
           set_te16(&h2->ehdr.e_shentsize, sizeof(Elf64_Shdr));
    if (o_elf_shnum) {
                        h2->ehdr.e_shnum = o_elf_shnum;
                        h2->ehdr.e_shstrndx = o_elf_shnum - 1;
    }
    else {
                        h2->ehdr.e_shnum = 0;
                        h2->ehdr.e_shstrndx = 0;
    }

    sz_elf_hdrs = sizeof(*h2) - sizeof(linfo);  // default
    if (gnu_stack) {
        sz_elf_hdrs += sizeof(Elf64_Phdr);
        memcpy(&h2->phdr[phnum_o++], gnu_stack, sizeof(*gnu_stack));
        set_te16(&h2->ehdr.e_phnum, phnum_o);
    }
    o_binfo =  sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr)*phnum_o + sizeof(l_info) + sizeof(p_info);
    set_te64(&h2->phdr[C_TEXT].p_filesz, sizeof(*h2));  // + identsize;
                  h2->phdr[C_TEXT].p_memsz = h2->phdr[C_TEXT].p_filesz;

    for (unsigned j=0; j < phnum_i; ++j) {
        if (PT_LOAD64==get_te32(&h3->phdr[j].p_type)) {
            set_te64(&h3->phdr[j].p_align, page_size);
        }
    }

    // Info for OS kernel to set the brk()
    if (brka) {
        // linux-2.6.14 binfmt_elf.c: SIGKILL if (0==.p_memsz) on a page boundary
        upx_uint64_t lo_va_user(~(upx_uint64_t)0);  // infinity
        for (int j= e_phnum; --j>=0; ) {
            if (PT_LOAD64 == get_te32(&phdri[j].p_type)) {
                upx_uint64_t const vaddr = get_te64(&phdri[j].p_vaddr);
                lo_va_user = umin64(lo_va_user, vaddr);
            }
        }
        set_te64(                 &h2->phdr[C_BASE].p_vaddr, lo_va_user);
        h2->phdr[C_BASE].p_paddr = h2->phdr[C_BASE].p_vaddr;
        h2->phdr[C_TEXT].p_vaddr = h2->phdr[C_BASE].p_vaddr;
        h2->phdr[C_TEXT].p_paddr = h2->phdr[C_BASE].p_vaddr;
        set_te32(&h2->phdr[C_BASE].p_type, PT_LOAD64);  // be sure
        h2->phdr[C_BASE].p_offset = 0;
        h2->phdr[C_BASE].p_filesz = 0;
        // .p_memsz = brka;  temporary until sz_pack2
        set_te64(&h2->phdr[C_BASE].p_memsz, brka - lo_va_user);
        set_te32(&h2->phdr[C_BASE].p_flags, Elf64_Phdr::PF_R | Elf64_Phdr::PF_W);
    }
    if (ph.format==getFormat()) {
        assert((2u+ !!gnu_stack) == phnum_o);
        set_te32(&h2->phdr[C_TEXT].p_flags, ~Elf64_Phdr::PF_W & get_te32(&h2->phdr[C_TEXT].p_flags));
        if (!gnu_stack) {
            memset(&h2->linfo, 0, sizeof(h2->linfo));
            fo->write(h2, sizeof(*h2));
        }
        else {
            memset(&h3->linfo, 0, sizeof(h3->linfo));
            fo->write(h3, sizeof(*h3));
        }
    }
    else {
        assert(false);  // unknown ph.format, PackLinuxElf64
    }
}