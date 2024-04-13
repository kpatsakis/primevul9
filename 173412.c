void PackLinuxElf64::unpack(OutputFile *fo)
{
    if (e_phoff != sizeof(Elf64_Ehdr)) {// Phdrs not contiguous with Ehdr
        throwCantUnpack("bad e_phoff");
    }
    unsigned const c_phnum = get_te16(&ehdri.e_phnum);
    unsigned u_phnum = 0;
    upx_uint64_t old_dtinit = 0;
    unsigned is_asl = 0;  // is Android Shared Library

    unsigned szb_info = sizeof(b_info);
    {
        upx_uint64_t const e_entry = get_te64(&ehdri.e_entry);
        if (e_entry < 0x401180
        &&  get_te16(&ehdri.e_machine)==Elf64_Ehdr::EM_386) { /* old style, 8-byte b_info */
            szb_info = 2*sizeof(unsigned);
        }
    }

    fi->seek(overlay_offset - sizeof(l_info), SEEK_SET);
    fi->readx(&linfo, sizeof(linfo));
    lsize = get_te16(&linfo.l_lsize);
    if (UPX_MAGIC_LE32 != get_le32(&linfo.l_magic)) {
        throwCantUnpack("l_info corrupted");
    }
    p_info hbuf;  fi->readx(&hbuf, sizeof(hbuf));
    unsigned orig_file_size = get_te32(&hbuf.p_filesize);
    blocksize = get_te32(&hbuf.p_blocksize);
    if ((u32_t)file_size > orig_file_size || blocksize > orig_file_size
        || !mem_size_valid(1, blocksize, OVERHEAD))
        throwCantUnpack("p_info corrupted");

    ibuf.alloc(blocksize + OVERHEAD);
    b_info bhdr; memset(&bhdr, 0, sizeof(bhdr));
    fi->readx(&bhdr, szb_info);
    ph.u_len = get_te32(&bhdr.sz_unc);
    ph.c_len = get_te32(&bhdr.sz_cpr);
    if (ph.c_len > (unsigned)file_size || ph.c_len == 0 || ph.u_len == 0
    ||  ph.u_len > orig_file_size)
        throwCantUnpack("b_info corrupted");
    ph.filter_cto = bhdr.b_cto8;

    MemBuffer u(ph.u_len);
    Elf64_Ehdr *const ehdr = (Elf64_Ehdr *)&u[0];
    Elf64_Phdr const *phdr = nullptr;
    total_in = 0;
    total_out = 0;
    unsigned c_adler = upx_adler32(nullptr, 0);
    unsigned u_adler = upx_adler32(nullptr, 0);

    unsigned is_shlib = 0;
    MemBuffer phdro;
    Elf64_Phdr const *const dynhdr = elf_find_ptype(Elf64_Phdr::PT_DYNAMIC, phdri, c_phnum);
    if (dynhdr) {
        upx_uint64_t dyn_offset = get_te64(&dynhdr->p_offset);
        upx_uint64_t dyn_filesz = get_te64(&dynhdr->p_filesz);
        dynseg = (Elf64_Dyn const *)ibuf.subref("bad DYNAMIC", dyn_offset, dyn_filesz);
        // Packed shlib? (ET_DYN without -fPIE)
        if (!(Elf64_Dyn::DF_1_PIE & elf_unsigned_dynamic(Elf64_Dyn::DT_FLAGS_1))) {
            is_shlib = 1;
            u_phnum = get_te16(&ehdri.e_phnum);
            phdro.alloc(u_phnum * sizeof(Elf64_Phdr));
            memcpy(phdro, phdri, u_phnum * sizeof(*phdri));
            un_shlib_1(fo, (Elf64_Phdr *)(void *)phdro, c_adler, u_adler, dynhdr, orig_file_size, szb_info);
            *ehdr = ehdri;
        }
    }
    else { // main executable
        // Uncompress Ehdr and Phdrs: info for control of unpacking
        if (ibuf.getSize() < ph.c_len)
            throwCompressedDataViolation();
        fi->readx(ibuf, ph.c_len);
        decompress(ibuf, (upx_byte *)ehdr, false);
        if (ehdr->e_type   !=ehdri.e_type
        ||  ehdr->e_machine!=ehdri.e_machine
        ||  ehdr->e_version!=ehdri.e_version
            // less strict for EM_PPC64 to workaround earlier bug
        ||  !( ehdr->e_flags==ehdri.e_flags
            || Elf64_Ehdr::EM_PPC64 == get_te16(&ehdri.e_machine))
        ||  ehdr->e_ehsize !=ehdri.e_ehsize
            // check EI_MAG[0-3], EI_CLASS, EI_DATA, EI_VERSION
        ||  memcmp(ehdr->e_ident, ehdri.e_ident, Elf64_Ehdr::EI_OSABI)) {
            throwCantUnpack("ElfXX_Ehdr corrupted");
        }
        // Rewind: prepare for data phase
        fi->seek(- (off_t) (szb_info + ph.c_len), SEEK_CUR);

        u_phnum = get_te16(&ehdr->e_phnum);
#define MAX_ELF_HDR 1024
        if ((umin64(MAX_ELF_HDR, ph.u_len) - sizeof(Elf64_Ehdr))/sizeof(Elf64_Phdr) < u_phnum) {
            throwCantUnpack("bad compressed e_phnum");
        }
        phdro.alloc(u_phnum * sizeof(Elf64_Phdr));
        memcpy(phdro, 1+ ehdr, u_phnum * sizeof(Elf64_Phdr));
#undef MAX_ELF_HDR

        // Decompress each PT_LOAD.
        bool first_PF_X = true;
        phdr = (Elf64_Phdr *) (void *) (1+ ehdr);  // uncompressed
        for (unsigned j=0; j < u_phnum; ++phdr, ++j) {
            if (PT_LOAD64==get_te32(&phdr->p_type)) {
                unsigned const filesz = get_te64(&phdr->p_filesz);
                unsigned const offset = get_te64(&phdr->p_offset);
                if (fo)
                    fo->seek(offset, SEEK_SET);
                if (Elf64_Phdr::PF_X & get_te32(&phdr->p_flags)) {
                    unpackExtent(filesz, fo,
                        c_adler, u_adler, first_PF_X, szb_info);
                    first_PF_X = false;
                }
                else {
                    unpackExtent(filesz, fo,
                        c_adler, u_adler, false, szb_info);
                }
            }
        }
    }

    phdr = phdri;
    load_va = 0;
    for (unsigned j=0; j < c_phnum; ++j) {
        if (PT_LOAD64==get_te32(&phdr->p_type)) {
            load_va = get_te64(&phdr->p_vaddr);
            break;
        }
    }
    if (0x1000==get_te64(&phdri[0].p_filesz)  // detect C_BASE style
    &&  0==get_te64(&phdri[1].p_offset)
    &&  0==get_te64(&phdri[0].p_offset)
    &&     get_te64(&phdri[1].p_filesz) == get_te64(&phdri[1].p_memsz)) {
        fi->seek(up4(get_te64(&phdr[1].p_memsz)), SEEK_SET);  // past the loader
    }
    else if (is_shlib
    ||  ((unsigned)(get_te64(&ehdri.e_entry) - load_va) + up4(lsize) +
                ph.getPackHeaderSize() + sizeof(overlay_offset))
            < up4(file_size)) {
        // Loader is not at end; skip past it.
        funpad4(fi);  // MATCH01
        unsigned d_info[6]; fi->readx(d_info, sizeof(d_info));
        if (0==old_dtinit) {
            old_dtinit = get_te32(&d_info[2 + (0==d_info[0])]);
            is_asl = 1u& get_te32(&d_info[0 + (0==d_info[0])]);
        }
        fi->seek(lsize - sizeof(d_info), SEEK_CUR);
    }

    // The gaps between PT_LOAD and after last PT_LOAD
    phdr = (Elf64_Phdr *)(void *)phdro;
    upx_uint64_t hi_offset(0);
    for (unsigned j = 0; j < u_phnum; ++j) {
        if (PT_LOAD64==phdr[j].p_type
        &&  hi_offset < phdr[j].p_offset)
            hi_offset = phdr[j].p_offset;
    }
    for (unsigned j = 0; j < u_phnum; ++j) {
        unsigned const size = find_LOAD_gap(phdr, j, u_phnum);
        if (size) {
            unsigned const where = get_te64(&phdr[j].p_offset) +
                                   get_te64(&phdr[j].p_filesz);
            if (fo)
                fo->seek(where, SEEK_SET);
            unpackExtent(size, fo,
                c_adler, u_adler, false, szb_info,
                is_shlib && ((phdr[j].p_offset != hi_offset)));
                // FIXME: should not depend on is_shlib ?
        }
    }

    // check for end-of-file
    fi->readx(&bhdr, szb_info);
    unsigned const sz_unc = ph.u_len = get_te32(&bhdr.sz_unc);

    if (sz_unc == 0) { // uncompressed size 0 -> EOF
        // note: magic is always stored le32
        unsigned const sz_cpr = get_le32(&bhdr.sz_cpr);
        if (sz_cpr != UPX_MAGIC_LE32)  // sz_cpr must be h->magic
            throwCompressedDataViolation();
    }
    else { // extra bytes after end?
        throwCompressedDataViolation();
    }

    if (is_shlib) {
        un_DT_INIT(old_dtinit, (Elf64_Phdr *)(void *)phdro, dynhdr, fo, is_asl);
    }

    // update header with totals
    ph.c_len = total_in;
    ph.u_len = total_out;

    // all bytes must be written
    if (fo && total_out != orig_file_size)
        throwEOFException();

    // finally test the checksums
    if (ph.c_adler != c_adler || ph.u_adler != u_adler)
        throwChecksumError();
}