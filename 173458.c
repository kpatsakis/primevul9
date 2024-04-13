off_t PackLinuxElf32::pack3(OutputFile *fo, Filter &ft)
{
    off_t flen = super::pack3(fo, ft);  // loader follows compressed PT_LOADs
    // NOTE: PackLinuxElf::pack3  adjusted xct_off for the extra page

    unsigned v_hole = sz_pack2 + lsize;
    set_te32(&elfout.phdr[C_TEXT].p_filesz, v_hole);
    set_te32(&elfout.phdr[C_TEXT].p_memsz,  v_hole);
    // Then compressed gaps (including debuginfo.)
    for (unsigned k = 0; k < e_phnum; ++k) {
        Extent x;
        x.size = find_LOAD_gap(phdri, k, e_phnum);
        if (x.size) {
            x.offset = get_te32(&phdri[k].p_offset) +
                       get_te32(&phdri[k].p_filesz);
            packExtent(x, nullptr, fo);
        }
    }
    // write block end marker (uncompressed size 0)
    b_info hdr; memset(&hdr, 0, sizeof(hdr));
    set_le32(&hdr.sz_cpr, UPX_MAGIC_LE32);
    fo->write(&hdr, sizeof(hdr));
    flen = fpad4(fo);

    set_te32(&elfout.phdr[C_TEXT].p_filesz, sz_pack2 + lsize);
    set_te32(&elfout.phdr[C_TEXT].p_memsz,  sz_pack2 + lsize);
    if (0==xct_off) { // not shared library
        set_te32(&elfout.phdr[C_BASE].p_align, 0u - page_mask);
        elfout.phdr[C_BASE].p_paddr = elfout.phdr[C_BASE].p_vaddr;
        elfout.phdr[C_BASE].p_offset = 0;
        // vbase handles ET_EXEC.  FIXME: pre-linking?
        unsigned vbase = get_te32(&elfout.phdr[C_BASE].p_vaddr);
        unsigned abrk = getbrk(phdri, e_phnum);
        set_te32(&elfout.phdr[C_BASE].p_filesz, 0x1000);  // Linux kernel SIGSEGV if (0==.p_filesz)
        set_te32(&elfout.phdr[C_BASE].p_memsz, abrk - vbase);
        set_te32(&elfout.phdr[C_BASE].p_flags, Elf32_Phdr::PF_W|Elf32_Phdr::PF_R);
        set_te32(&elfout.phdr[C_TEXT].p_vaddr, abrk= (page_mask & (~page_mask + abrk)));
        elfout.phdr[C_TEXT].p_paddr = elfout.phdr[C_TEXT].p_vaddr;
        set_te32(&elfout.ehdr.e_entry, abrk + get_te32(&elfout.ehdr.e_entry) - vbase);
    }
    if (0!=xct_off) {  // shared library
        unsigned word = (Elf32_Ehdr::EM_ARM==e_machine) + load_va + sz_pack2;  // Thumb mode
        set_te32(&file_image[user_init_off], word);  // set the hook

        Elf32_Phdr *phdr = (Elf32_Phdr *)lowmem.subref(
                "bad e_phoff", e_phoff, e_phnum * sizeof(Elf32_Phdr));
        unsigned off = fo->st_size();
        so_slide = 0;
        for (unsigned j = 0; j < e_phnum; ++j, ++phdr) {
            unsigned const len  = get_te32(&phdr->p_filesz);
            unsigned const ioff = get_te32(&phdr->p_offset);
            unsigned       align= get_te32(&phdr->p_align);
            unsigned const type = get_te32(&phdr->p_type);
            if (Elf32_Phdr::PT_INTERP==type) {
                // Rotate to highest position, so it can be lopped
                // by decrementing e_phnum.
                memcpy((unsigned char *)ibuf, phdr, sizeof(*phdr));  // extract
                memmove(phdr, 1+phdr, (e_phnum - (1+ j))*sizeof(*phdr));  // overlapping
                memcpy(&phdr[e_phnum - (1+ j)], (unsigned char *)ibuf, sizeof(*phdr));  // to top
                --phdr; --e_phnum;
                set_te16(&ehdri.e_phnum, e_phnum);
                set_te16(&((Elf32_Ehdr *)(unsigned char *)lowmem)->e_phnum, e_phnum);
                continue;
            }
            if (PT_LOAD32 == type) {
                if ((xct_off - ioff) < len) { // Change length of compressed PT_LOAD.
                    set_te32(&phdr->p_filesz, sz_pack2 + lsize - ioff);
                    set_te32(&phdr->p_memsz,  sz_pack2 + lsize - ioff);
                    if (user_init_off < xct_off) { // MIPS puts PT_DYNAMIC here
                        // Allow for DT_INIT in a new [stolen] slot
                        unsigned off2 = user_init_off - sizeof(word);
                        fo->seek(off2, SEEK_SET);
                        fo->rewrite(&file_image[off2], 2*sizeof(word));
                    }
                }
                else if (xct_off < ioff) { // Slide subsequent PT_LOAD.
                    if ((1u<<12) < align) {
                        align = 1u<<12;
                        set_te32(&phdr->p_align, align);
                    }
                    off += (align-1) & (ioff - off);
                    fo->seek(  off, SEEK_SET);
                    fo->write(&file_image[ioff], len);
                    so_slide = off - ioff;
                    set_te32(&phdr->p_offset, so_slide + ioff);
                }
                continue;  // all done with this PT_LOAD
            }
            if (xct_off < ioff)
                set_te32(&phdr->p_offset, so_slide + ioff);
        }  // end each Phdr

        if (opt->o_unix.android_shlib) {
            // Update {DYNAMIC}.sh_offset by so_slide.
            Elf32_Shdr *shdr = (Elf32_Shdr *)lowmem.subref(
                    "bad e_shoff", xct_off - asl_delta, e_shnum * sizeof(Elf32_Shdr));
            for (unsigned j = 0; j < e_shnum; ++shdr, ++j) {
                unsigned sh_type = get_te32(&shdr->sh_type);
                if (Elf32_Shdr::SHT_DYNAMIC == get_te32(&shdr->sh_type)) {
                    unsigned offset = get_te32(&shdr->sh_offset);
                    set_te32(&shdr->sh_offset, so_slide + offset );
                    fo->seek((j * sizeof(Elf32_Shdr)) + xct_off - asl_delta, SEEK_SET);
                    fo->rewrite(shdr, sizeof(*shdr));
                    fo->seek(0, SEEK_END);
                }
                if (Elf32_Shdr::SHT_REL == sh_type
                &&  n_jmp_slot
                &&  !strcmp(".rel.plt", get_te32(&shdr->sh_name) + shstrtab)) {
                    unsigned f_off = elf_get_offset_from_address(plt_off);
                    fo->seek(so_slide + f_off, SEEK_SET);  // FIXME: assumes PT_LOAD[1]
                    fo->rewrite(&file_image[f_off], n_jmp_slot * 4);
                 }
            }
        }
        else { // !opt->o_unix.android_shlib)
            ehdri.e_shnum = 0;
            ehdri.e_shoff = 0;
            ehdri.e_shstrndx = 0;
        }
    }
    return flen;
}