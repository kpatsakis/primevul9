void PackLinuxElf64::un_shlib_1(
    OutputFile *const fo,
    Elf64_Phdr *const phdro,
    unsigned &c_adler,
    unsigned &u_adler,
    Elf64_Phdr const *const dynhdr,
    unsigned const orig_file_size,
    unsigned const szb_info
)
{
    // The first PT_LOAD.  Part is not compressed (for benefit of rtld.)
    fi->seek(0, SEEK_SET);
    unsigned const limit_dynhdr = get_te64(&dynhdr->p_offset) + get_te64(&dynhdr->p_filesz);
    fi->readx(ibuf, limit_dynhdr);
    overlay_offset -= sizeof(linfo);
    xct_off = overlay_offset;
    e_shoff = get_te64(&ehdri.e_shoff);
    if (e_shoff && e_shnum
    &&  (e_shoff + sizeof(Elf64_Shdr) * e_shnum) <= limit_dynhdr) { // --android-shlib
        ibuf.subref("bad .e_shoff %#lx for %#lx", e_shoff, sizeof(Elf64_Shdr) * e_shnum);
        shdri = (Elf64_Shdr /*const*/ *)ibuf.subref(
            "bad Shdr table", e_shoff, sizeof(Elf64_Shdr)*e_shnum);
        upx_uint64_t xct_off2 = get_te64(&shdri->sh_offset);
        if (e_shoff == xct_off2) {
            xct_off = e_shoff;
        }
        // un-Relocate dynsym (DT_SYMTAB) which is below xct_off
        dynstr = (char const *)elf_find_dynamic(Elf64_Dyn::DT_STRTAB);
        sec_dynsym = elf_find_section_type(Elf64_Shdr::SHT_DYNSYM);
        if (sec_dynsym) {
            upx_uint64_t const off_dynsym = get_te64(&sec_dynsym->sh_offset);
            upx_uint64_t const sz_dynsym  = get_te64(&sec_dynsym->sh_size);
            if (orig_file_size < sz_dynsym
            ||  orig_file_size < off_dynsym
            || (orig_file_size - off_dynsym) < sz_dynsym) {
                throwCantUnpack("bad SHT_DYNSYM");
            }
            Elf64_Sym *const sym0 = (Elf64_Sym *)ibuf.subref(
                "bad dynsym", off_dynsym, sz_dynsym);
            Elf64_Sym *sym = sym0;
            for (int j = sz_dynsym / sizeof(Elf64_Sym); --j>=0; ++sym) {
                upx_uint64_t symval = get_te64(&sym->st_value);
                unsigned symsec = get_te16(&sym->st_shndx);
                if (Elf64_Sym::SHN_UNDEF != symsec
                &&  Elf64_Sym::SHN_ABS   != symsec
                &&  xct_off <= symval) {
                    set_te64(&sym->st_value, symval - asl_delta);
                }
                if (Elf64_Sym::SHN_ABS == symsec && xct_off <= symval) {
                    adjABS(sym, 0u - asl_delta);
                }
            }
        }
    }
    if (fo) {
        fo->write(ibuf, xct_off);
    }
    total_in  = xct_off;
    total_out = xct_off;

    // Decompress and unfilter the tail of PT_LOAD containing xct_off
    fi->seek(xct_off + sizeof(linfo) + sizeof(p_info), SEEK_SET);
    struct b_info hdr;
    fi->readx(&hdr, sizeof(hdr));  // Peek at b_info
    fi->seek(-(off_t)sizeof(hdr), SEEK_CUR);
    ph.c_len = get_te32(&hdr.sz_cpr);
    ph.u_len = get_te32(&hdr.sz_unc);

    unpackExtent(ph.u_len, fo,
        c_adler, u_adler, false, szb_info);

    // Copy (slide) the remaining PT_LOAD; they are not compressed
    Elf64_Phdr *phdr = phdro;
    unsigned slide = 0;
    int first = 0;
    for (unsigned k = 0; k < e_phnum; ++k, ++phdr) {
        unsigned type = get_te32(&phdr->p_type);
        unsigned vaddr = get_te64(&phdr->p_vaddr);
        unsigned offset = get_te64(&phdr->p_offset);
        unsigned filesz = get_te64(&phdr->p_filesz);
        if (xct_off <= vaddr) {
            if (PT_LOAD64==type) {
                if (0==first++) { // the partially-compressed PT_LOAD
                    set_te64(&phdr->p_filesz, ph.u_len + xct_off - vaddr);
                    set_te64(&phdr->p_memsz,  ph.u_len + xct_off - vaddr);
                }
                else { // subsequent PT_LOAD
                    fi->seek(offset, SEEK_SET);
                    fi->readx(ibuf, filesz);
                    total_in += filesz;

                    if (0==slide) {
                        slide = vaddr - offset;
                    }
                    if (fo) {
                        fo->seek(slide + offset, SEEK_SET);
                        fo->write(ibuf, filesz);
                        total_out = filesz + slide + offset;  // high-water mark
                    }
                }
            }
            set_te64(&phdr->p_offset, slide + offset);
        }
    }
    if (fo) { // Rewrite Phdrs after sliding
        fo->seek(sizeof(Elf64_Ehdr), SEEK_SET);
        fo->rewrite(phdro, e_phnum * sizeof(Elf64_Phdr));
    }
    // loader offset
    fi->seek(xct_off + sizeof(linfo) + sizeof(p_info) + sizeof(b_info) + up4(ph.c_len), SEEK_SET);
}