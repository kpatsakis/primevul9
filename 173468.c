void PackLinuxElf32::pack1(OutputFile *fo, Filter & /*ft*/)
{
    fi->seek(0, SEEK_SET);
    fi->readx(&ehdri, sizeof(ehdri));
    assert(e_phoff == sizeof(Elf32_Ehdr));  // checked by canPack()
    sz_phdrs = e_phnum * get_te16(&ehdri.e_phentsize);

    // Remember all PT_NOTE, and find lg2_page from PT_LOAD.
    Elf32_Phdr *phdr = phdri;
    note_size = 0;
    for (unsigned j=0; j < e_phnum; ++phdr, ++j) {
        if (PT_NOTE32 == get_te32(&phdr->p_type)) {
            note_size += up4(get_te32(&phdr->p_filesz));
        }
    }
    if (note_size) {
        note_body.alloc(note_size);
        note_size = 0;
    }
    phdr = phdri;
    for (unsigned j=0; j < e_phnum; ++phdr, ++j) {
        unsigned const type = get_te32(&phdr->p_type);
        if (PT_NOTE32 == type) {
            unsigned const len = get_te32(&phdr->p_filesz);
            fi->seek(get_te32(&phdr->p_offset), SEEK_SET);
            fi->readx(&note_body[note_size], len);
            note_size += up4(len);
        }
        if (PT_LOAD32 == type) {
            unsigned x = get_te32(&phdr->p_align) >> lg2_page;
            while (x>>=1) {
                ++lg2_page;
            }
        }
        if (PT_GNU_STACK32 == type) {
            // MIPS stub cannot handle GNU_STACK yet.
            if (Elf32_Ehdr::EM_MIPS != this->e_machine) {
                gnu_stack = phdr;
            }
        }
    }
    page_size =  1u<<lg2_page;
    page_mask = ~0u<<lg2_page;

    progid = 0;  // getRandomId();  not useful, so do not clutter
    sz_elf_hdrs = sizeof(ehdri) + sz_phdrs;
    if (0!=xct_off) {  // shared library
        sz_elf_hdrs = xct_off;
        lowmem.alloc(xct_off + (!opt->o_unix.android_shlib
            ? 0
            : e_shnum * sizeof(Elf32_Shdr)));
        memcpy(lowmem, file_image, xct_off);  // android omits Shdr here
        fo->write(lowmem, xct_off);  // < SHF_EXECINSTR (typ: in .plt or .init)
        if (opt->o_unix.android_shlib) {
            // In order to pacify the runtime linker on Android "O" ("Oreo"),
            // we will splice-in a 4KiB page that contains an "extra" copy
            // of the Shdr, any PT_NOTE above xct_off, and shstrtab.
            // File order: Ehdr, Phdr[], section contents below xct_off,
            //    Shdr_copy[], PT_NOTEs.hi, shstrtab.
            xct_va  += asl_delta;
            //xct_off += asl_delta;  // not yet

            // Relocate PT_DYNAMIC (in 2nd PT_LOAD)
            Elf32_Dyn *dyn = const_cast<Elf32_Dyn *>(dynseg);
            for (; dyn->d_tag; ++dyn) {
                unsigned d_tag = get_te32(&dyn->d_tag);
                if (Elf32_Dyn::DT_FINI       == d_tag
                ||  Elf32_Dyn::DT_FINI_ARRAY == d_tag
                ||  Elf32_Dyn::DT_INIT_ARRAY == d_tag
                ||  Elf32_Dyn::DT_PREINIT_ARRAY == d_tag
                ||  Elf32_Dyn::DT_PLTGOT     == d_tag) {
                    unsigned d_val = get_te32(&dyn->d_val);
                    set_te32(&dyn->d_val, asl_delta + d_val);
                }
            }

            // Relocate dynsym (DT_SYMTAB) which is below xct_va
            unsigned const off_dynsym = get_te32(&sec_dynsym->sh_offset);
            unsigned const sz_dynsym  = get_te32(&sec_dynsym->sh_size);
            Elf32_Sym *dyntym = (Elf32_Sym *)lowmem.subref(
                "bad dynsym", off_dynsym, sz_dynsym);
            Elf32_Sym *sym = dyntym;
            for (int j = sz_dynsym / sizeof(Elf32_Sym); --j>=0; ++sym) {
                unsigned symval = get_te32(&sym->st_value);
                unsigned symsec = get_te16(&sym->st_shndx);
                if (Elf32_Sym::SHN_UNDEF != symsec
                &&  Elf32_Sym::SHN_ABS   != symsec
                &&  xct_off <= symval) {
                    set_te32(&sym->st_value, asl_delta + symval);
                }
                if (Elf32_Sym::SHN_ABS == symsec && xct_off <= symval) {
                    adjABS(sym, asl_delta);
                }
            }

            // Relocate Phdr virtual addresses, but not physical offsets and sizes
            unsigned char buf_notes[512]; memset(buf_notes, 0, sizeof(buf_notes));
            unsigned len_notes = 0;
            phdr = (Elf32_Phdr *)lowmem.subref(
                "bad e_phoff", e_phoff, e_phnum * sizeof(Elf32_Phdr));
            for (unsigned j = 0; j < e_phnum; ++j, ++phdr) {
                upx_uint32_t offset = get_te32(&phdr->p_offset);
                if (xct_off <= offset) { // above the extra page
                    if (PT_NOTE32 == get_te32(&phdr->p_type)) {
                        upx_uint32_t memsz = get_te32(&phdr->p_memsz);
                        if (sizeof(buf_notes) < (memsz + len_notes)) {
                            throwCantPack("PT_NOTEs too big");
                        }
                        set_te32(&phdr->p_vaddr,
                            len_notes + (e_shnum * sizeof(Elf32_Shdr)) + xct_off);
                        phdr->p_offset = phdr->p_paddr = phdr->p_vaddr;
                        memcpy(&buf_notes[len_notes], &file_image[offset], memsz);
                        len_notes += memsz;
                    }
                    else {
                        //set_te32(&phdr->p_offset, asl_delta + offset);  // physical
                        upx_uint32_t addr = get_te32(&phdr->p_paddr);
                        set_te32(&phdr->p_paddr, asl_delta + addr);
                                     addr = get_te32(&phdr->p_vaddr);
                        set_te32(&phdr->p_vaddr, asl_delta + addr);
                    }
                }
                // .p_filesz,.p_memsz are updated in ::pack3
            }

            Elf32_Ehdr *const ehdr = (Elf32_Ehdr *)&lowmem[0];
            upx_uint32_t e_entry = get_te32(&ehdr->e_entry);
            if (xct_off < e_entry) {
                set_te32(&ehdr->e_entry, asl_delta + e_entry);
            }
            // Relocate Shdr; and Rela, Rel (below xct_off)
            set_te32(&ehdr->e_shoff, xct_off);
            memcpy(&lowmem[xct_off], shdri, e_shnum * sizeof(Elf32_Shdr));
            Elf32_Shdr *const shdro = (Elf32_Shdr *)&lowmem[xct_off];
            Elf32_Shdr *shdr = shdro;
            unsigned sz_shstrtab  = get_te32(&sec_strndx->sh_size);
            for (unsigned j = 0; j < e_shnum; ++j, ++shdr) {

                unsigned sh_type = get_te32(&shdr->sh_type);
                unsigned sh_size = get_te32(&shdr->sh_size);
                unsigned  sh_offset = get_te32(&shdr->sh_offset);
                unsigned sh_entsize = get_te32(&shdr->sh_entsize);
                if (xct_off <= sh_offset) {
                    //set_te32(&shdr->sh_offset, asl_delta + sh_offset);  // FIXME ??
                    upx_uint32_t addr = get_te32(&shdr->sh_addr);
                    set_te32(&shdr->sh_addr, asl_delta + addr);
                }
                if (Elf32_Shdr::SHT_RELA== sh_type) {
                    if (sizeof(Elf32_Rela) != sh_entsize) {
                        char msg[50];
                        snprintf(msg, sizeof(msg), "bad Rela.sh_entsize %u", sh_entsize);
                        throwCantPack(msg);
                    }
                    n_jmp_slot = 0;
                    plt_off = ~0u;
                    Elf32_Rela *const relb = (Elf32_Rela *)lowmem.subref(
                         "bad Rela offset", sh_offset, sh_size);
                    Elf32_Rela *rela = relb;
                    for (int k = sh_size / sh_entsize; --k >= 0; ++rela) {
                        unsigned r_addend = get_te32(&rela->r_addend);
                        unsigned r_offset = get_te32(&rela->r_offset);
                        unsigned r_info   = get_te32(&rela->r_info);
                        unsigned r_type = ELF32_R_TYPE(r_info);
                        if (xct_off <= r_offset) {
                            set_te32(&rela->r_offset, asl_delta + r_offset);
                        }
                        if (Elf32_Ehdr::EM_ARM == e_machine) {
                            if (R_ARM_RELATIVE == r_type) {
                                if (xct_off <= r_addend) {
                                    set_te32(&rela->r_addend, asl_delta + r_addend);
                                }
                            }
                            if (R_ARM_JUMP_SLOT == r_type) {
                                // .rela.plt contains offset of the "first time" target
                                if (plt_off > r_offset) {
                                    plt_off = r_offset;
                                }
                                unsigned d = elf_get_offset_from_address(r_offset);
                                unsigned w = get_te32(&file_image[d]);
                                if (xct_off <= w) {
                                    set_te32(&file_image[d], asl_delta + w);
                                }
                                ++n_jmp_slot;
                            }
                        }
                    }
                    fo->seek(sh_offset, SEEK_SET);
                    fo->rewrite(relb, sh_size);
                }
                if (Elf32_Shdr::SHT_REL == sh_type) {
                    if (sizeof(Elf32_Rel) != sh_entsize) {
                        char msg[50];
                        snprintf(msg, sizeof(msg), "bad Rel.sh_entsize %u", sh_entsize);
                        throwCantPack(msg);
                    }
                    n_jmp_slot = 0;
                    plt_off = ~0u;
                    Elf32_Rel *const rel0 = (Elf32_Rel *)lowmem.subref(
                         "bad Rel offset", sh_offset, sh_size);
                    Elf32_Rel *rel = rel0;
                    for (int k = sh_size / sh_entsize; --k >= 0; ++rel) {
                        unsigned r_offset = get_te32(&rel->r_offset);
                        unsigned r_info = get_te32(&rel->r_info);
                        unsigned r_type = ELF32_R_TYPE(r_info);
                        unsigned d = elf_get_offset_from_address(r_offset);
                        unsigned w = get_te32(&file_image[d]);
                        if (xct_off <= r_offset) {
                            set_te32(&rel->r_offset, asl_delta + r_offset);
                        }
                        if (Elf32_Ehdr::EM_ARM == e_machine) switch (r_type) {
                            default: {
                                char msg[90]; snprintf(msg, sizeof(msg),
                                    "unexpected relocation %#x [%#x]",
                                    r_type, -1 + (sh_size / sh_entsize) - k);
                                throwCantPack(msg);
                            } break;
                            case R_ARM_ABS32:  // FALL THROUGH
                            case R_ARM_GLOB_DAT: // FALL THROUGH
                            case R_ARM_RELATIVE: {
                                if (xct_off <= w) {
                                    set_te32(&file_image[d], asl_delta + w);
                                }
                            } break;
                            case R_ARM_JUMP_SLOT: {
                                if (plt_off > r_offset) {
                                    plt_off = r_offset;
                                }
                                if (xct_off <= w) {
                                    set_te32(&file_image[d], asl_delta + w);
                                }
                                ++n_jmp_slot;
                            }; break;
                        }
                    }
                    fo->seek(sh_offset, SEEK_SET);
                    fo->rewrite(rel0, sh_size);
                }
                if (Elf32_Shdr::SHT_NOTE == sh_type) {
                    if (!(Elf32_Shdr::SHF_ALLOC & get_te32(&shdr->sh_flags))) {
                        // example: version number of 'gold' linker (static binder)
                        if (sizeof(buf_notes) < (sh_size + len_notes)) {
                            throwCantPack("SHT_NOTEs too big");
                        }
                        set_te32(&shdro[j].sh_offset,
                            len_notes + (e_shnum * sizeof(Elf32_Shdr)) + xct_off);
                        memcpy(&buf_notes[len_notes], &file_image[sh_offset], sh_size);
                        len_notes += sh_size;
                    }
                    else { // SHF_ALLOC, thus already in PT_LOAD
                        // Not sure why we need this conditional.
                        // Anyway, some Android have multiple SHT_NOTE sections.
                        if (xct_off <= sh_offset) {
                            upx_uint32_t pos = xct_off + e_shnum * sizeof(Elf32_Shdr);
                            set_te32(&shdr->sh_addr,   pos);
                            set_te32(&shdr->sh_offset, pos);
                        }
                    }
                }
            }
            // shstrndx will move
            set_te32(&shdro[get_te16(&ehdri.e_shstrndx)].sh_offset,
                len_notes + e_shnum * sizeof(Elf32_Shdr) + xct_off);

            // (Re-)write all changes below xct_off
            fo->seek(0, SEEK_SET);
            fo->rewrite(lowmem, xct_off);

            // New copy of Shdr
            Elf32_Shdr blank; memset(&blank, 0, sizeof(blank));
            set_te32(&blank.sh_offset, xct_off);  // hint for "upx -d"
            fo->write(&blank, sizeof(blank));
            fo->write(&shdro[1], (-1+ e_shnum) * sizeof(Elf32_Shdr));

            if (len_notes) {
                fo->write(buf_notes, len_notes);
            }

            // New copy of Shdr[.e_shstrndx].[ sh_offset, +.sh_size )
            fo->write(shstrtab,  sz_shstrtab);

            sz_elf_hdrs = fpad4(fo);
            //xct_off += asl_delta;  // wait until ::pack3
        }
        memset(&linfo, 0, sizeof(linfo));
        fo->write(&linfo, sizeof(linfo));
    }

    // if the preserve build-id option was specified
    if (opt->o_unix.preserve_build_id) {
        // set this so we can use elf_find_section_name
        e_shnum = get_te16(&ehdri.e_shnum);
        MemBuffer mb_shdri;
        if (!shdri) {
            mb_shdri.alloc(e_shnum * sizeof(Elf32_Shdr));
            shdri = (Elf32_Shdr *)mb_shdri.getVoidPtr();
            e_shoff = get_te32(&ehdri.e_shoff);
            fi->seek(e_shoff, SEEK_SET);
            fi->readx(shdri, e_shnum * sizeof(Elf32_Shdr));
        }
        //set the shstrtab
        sec_strndx = &shdri[get_te16(&ehdri.e_shstrndx)];

        unsigned sh_size = get_te32(&sec_strndx->sh_size);
        mb_shstrtab.alloc(sh_size); shstrtab = (char *)mb_shstrtab.getVoidPtr();
        fi->seek(0,SEEK_SET);
        fi->seek(sec_strndx->sh_offset,SEEK_SET);
        fi->readx(mb_shstrtab, sh_size);

        Elf32_Shdr const *buildid = elf_find_section_name(".note.gnu.build-id");
        if (buildid) {
            unsigned bid_sh_size = get_te32(&buildid->sh_size);
            buildid_data.alloc(bid_sh_size);
            buildid_data.clear();
            fi->seek(0,SEEK_SET);
            fi->seek(buildid->sh_offset,SEEK_SET);
            fi->readx((void *)buildid_data, bid_sh_size);

            o_elf_shnum = 3;
            memset(&shdrout,0,sizeof(shdrout));

            //setup the build-id
            memcpy(&shdrout.shdr[1], buildid, sizeof(shdrout.shdr[1]));
            set_te32(&shdrout.shdr[1].sh_name, 1);

            //setup the shstrtab
            memcpy(&shdrout.shdr[2], sec_strndx, sizeof(shdrout.shdr[2]));
            set_te32(&shdrout.shdr[2].sh_name, 20);
            set_te32(&shdrout.shdr[2].sh_size, 29); //size of our static shstrtab
        }
    }
}