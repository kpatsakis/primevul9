PackNetBSDElf32x86::generateElfHdr(
    OutputFile *fo,
    void const *proto,
    unsigned const brka
)
{
    super::generateElfHdr(fo, proto, brka);
    cprElfHdr2 *const h2 = (cprElfHdr2 *)(void *)&elfout;

    sz_elf_hdrs = sizeof(*h2) - sizeof(linfo);
    unsigned note_offset = sz_elf_hdrs;

    // Find the NetBSD PT_NOTE and the PaX PT_NOTE.
    Elf32_Nhdr const *np_NetBSD = nullptr;  unsigned sz_NetBSD = 0;
    Elf32_Nhdr const *np_PaX    = nullptr;  unsigned sz_PaX    = 0;
    unsigned char *cp = (unsigned char *)note_body;
    unsigned j;
    for (j=0; j < note_size; ) {
        Elf32_Nhdr const *const np = (Elf32_Nhdr const *)(void *)cp;
        int k = sizeof(*np) + up4(get_te32(&np->namesz))
            + up4(get_te32(&np->descsz));

        if (NHDR_NETBSD_TAG == np->type && 7== np->namesz
        &&  NETBSD_DESCSZ == np->descsz
        &&  0==strcmp(ELF_NOTE_NETBSD_NAME,
                /* &np->body */ (char const *)(1+ np))) {
            np_NetBSD = np;
            sz_NetBSD = k;
        }
        if (NHDR_PAX_TAG == np->type && 4== np->namesz
        &&  PAX_DESCSZ==np->descsz
        &&  0==strcmp(ELF_NOTE_PAX_NAME,
                /* &np->body */ (char const *)(1+ np))) {
            np_PaX = np;
            sz_PaX = k;
        }
        cp += k;
        j += k;
    }

    // Add PT_NOTE for the NetBSD note and PaX note, if any.
    note_offset += (np_NetBSD ? sizeof(Elf32_Phdr) : 0);
    note_offset += (np_PaX    ? sizeof(Elf32_Phdr) : 0);
    Elf32_Phdr *phdr = &elfout.phdr[C_NOTE];
    if (np_NetBSD) {
        set_te32(&phdr->p_type, PT_NOTE32);
        set_te32(&phdr->p_offset, note_offset);
        set_te32(&phdr->p_vaddr, note_offset);
        set_te32(&phdr->p_paddr, note_offset);
        set_te32(&phdr->p_filesz, sz_NetBSD);
        set_te32(&phdr->p_memsz,  sz_NetBSD);
        set_te32(&phdr->p_flags, Elf32_Phdr::PF_R);
        set_te32(&phdr->p_align, 4);

        sz_elf_hdrs += sz_NetBSD + sizeof(*phdr);
        note_offset += sz_NetBSD;
        ++phdr;
    }
    if (np_PaX) {
        set_te32(&phdr->p_type, PT_NOTE32);
        set_te32(&phdr->p_offset, note_offset);
        set_te32(&phdr->p_vaddr, note_offset);
        set_te32(&phdr->p_paddr, note_offset);
        set_te32(&phdr->p_filesz, sz_PaX);
        set_te32(&phdr->p_memsz,  sz_PaX);
        set_te32(&phdr->p_flags, Elf32_Phdr::PF_R);
        set_te32(&phdr->p_align, 4);

        /* &np_PaX->body[4] */
        const unsigned char *p4 =  &(ACC_CCAST(const unsigned char *, (1+ np_PaX)))[4];
        unsigned bits = get_te32(p4);
        bits &= ~PAX_MPROTECT;
        bits |=  PAX_NOMPROTECT;
        set_te32(ACC_UNCONST_CAST(unsigned char *, p4), bits);

        sz_elf_hdrs += sz_PaX + sizeof(*phdr);
        note_offset += sz_PaX;
        ++phdr;
    }
    set_te32(&h2->phdr[C_TEXT].p_filesz, note_offset);
              h2->phdr[C_TEXT].p_memsz = h2->phdr[C_TEXT].p_filesz;

    if (ph.format==getFormat()) {
        set_te16(&h2->ehdr.e_phnum, !!sz_NetBSD + !!sz_PaX +
        get_te16(&h2->ehdr.e_phnum));
        fo->seek(0, SEEK_SET);
        fo->rewrite(h2, sizeof(*h2) - sizeof(h2->linfo));

        // The 'if' guards on these two calls to memcpy are required
        // because the C Standard Committee did not debug the Standard
        // before publishing.  An empty region (0==size) must nevertheless
        // have a valid (non-nullptr) pointer.
        if (sz_NetBSD) memcpy(&((char *)phdr)[0],         np_NetBSD, sz_NetBSD);
        if (sz_PaX)    memcpy(&((char *)phdr)[sz_NetBSD], np_PaX,    sz_PaX);

        fo->write(&elfout.phdr[C_NOTE],
            &((char *)phdr)[sz_PaX + sz_NetBSD] - (char *)&elfout.phdr[C_NOTE]);

        l_info foo; memset(&foo, 0, sizeof(foo));
        fo->rewrite(&foo, sizeof(foo));
    }
    else {
        assert(false);  // unknown ph.format, PackLinuxElf32
    }
}