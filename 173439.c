void PackLinuxElf32x86::addStubEntrySections(Filter const *ft)
{
    int const n_mru = ft->n_mru;  // FIXME: belongs to filter? packerf?

// Rely on "+80CXXXX" [etc] in getDecompressorSections() packer_c.cpp */
//    // Here is a quick summary of the format of the output file:
//    linker->setLoaderAlignOffset(
//            // Elf32_Ehdr
//        sizeof(elfout.ehdr) +
//            // Elf32_Phdr: 1 for exec86, 2 for sh86, 3 for elf86
//        (get_te16(&elfout.ehdr.e_phentsize) * get_te16(&elfout.ehdr.e_phnum)) +
//            // checksum UPX! lsize version format
//        sizeof(l_info) +
//            // PT_DYNAMIC with DT_NEEDED "forwarded" from original file
//        ((get_te16(&elfout.ehdr.e_phnum)==3)
//            ? (unsigned) get_te32(&elfout.phdr[C_NOTE].p_memsz)
//            : 0) +
//            // p_progid, p_filesize, p_blocksize
//        sizeof(p_info) +
//            // compressed data
//        b_len + ph.c_len );

            // entry to stub
    addLoader("LEXEC000", nullptr);

    if (ft->id) {
        { // decompr, unfilter are separate
            addLoader("LXUNF000", nullptr);
            addLoader("LXUNF002", nullptr);
                if (0x80==(ft->id & 0xF0)) {
                    if (256==n_mru) {
                        addLoader("MRUBYTE0", nullptr);
                    }
                    else if (n_mru) {
                        addLoader("LXMRU005", nullptr);
                    }
                    if (n_mru) {
                        addLoader("LXMRU006", nullptr);
                    }
                    else {
                        addLoader("LXMRU007", nullptr);
                    }
            }
            else if (0x40==(ft->id & 0xF0)) {
                addLoader("LXUNF008", nullptr);
            }
            addLoader("LXUNF010", nullptr);
        }
        if (n_mru) {
            addLoader("LEXEC009", nullptr);
        }
    }
    addLoader("LEXEC010", nullptr);
    addLoader(getDecompressorSections(), nullptr);
    addLoader("LEXEC015", nullptr);
    if (ft->id) {
        {  // decompr, unfilter are separate
            if (0x80!=(ft->id & 0xF0)) {
                addLoader("LXUNF042", nullptr);
            }
        }
        addFilter32(ft->id);
        { // decompr, unfilter are separate
            if (0x80==(ft->id & 0xF0)) {
                if (0==n_mru) {
                    addLoader("LXMRU058", nullptr);
                }
            }
            addLoader("LXUNF035", nullptr);
        }
    }
    else {
        addLoader("LEXEC017", nullptr);
    }

    addLoader("IDENTSTR", nullptr);
    addLoader("LEXEC020", nullptr);
    addLoader("FOLDEXEC", nullptr);
}