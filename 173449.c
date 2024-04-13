int PackLinuxElf64::pack2(OutputFile *fo, Filter &ft)
{
    Extent x;
    unsigned k;
    bool const is_shlib = (0!=xct_off);

    // count passes, set ptload vars
    uip->ui_total_passes = 0;
    for (k = 0; k < e_phnum; ++k) {
        if (PT_LOAD64==get_te32(&phdri[k].p_type)) {
            uip->ui_total_passes++;
            if (find_LOAD_gap(phdri, k, e_phnum)) {
                uip->ui_total_passes++;
            }
        }
    }
    uip->ui_total_passes -= !!is_shlib;  // not .data of shlib

    // compress extents
    unsigned hdr_u_len = (is_shlib ? xct_off : (sizeof(Elf64_Ehdr) + sz_phdrs));

    total_in =  (is_shlib ?           0 : xct_off);
    total_out = (is_shlib ? sz_elf_hdrs : xct_off);

    uip->ui_pass = 0;
    ft.addvalue = 0;

    unsigned nk_f = 0; upx_uint64_t xsz_f = 0;
    for (k = 0; k < e_phnum; ++k)
    if (PT_LOAD64==get_te32(&phdri[k].p_type)
    &&  Elf64_Phdr::PF_X & get_te64(&phdri[k].p_flags)) {
        upx_uint64_t xsz = get_te64(&phdri[k].p_filesz);
        if (xsz_f < xsz) {
            xsz_f = xsz;
            nk_f = k;
        }
    }
    int nx = 0;
    for (k = 0; k < e_phnum; ++k)
    if (PT_LOAD64==get_te32(&phdri[k].p_type)) {
        if (ft.id < 0x40) {
            // FIXME: ??    ft.addvalue = phdri[k].p_vaddr;
        }
        x.offset = get_te64(&phdri[k].p_offset);
        x.size   = get_te64(&phdri[k].p_filesz);
        if (!is_shlib || hdr_u_len < (u64_t)x.size) {
            if (0 == nx) { // 1st PT_LOAD64 must cover Ehdr at 0==p_offset
                unsigned const delta = hdr_u_len;
                if (ft.id < 0x40) {
                    // FIXME: ??     ft.addvalue += asl_delta;
                }
                if ((off_t)delta == x.size) { // PT_LOAD[0] with ElfXX.Ehdr only
                    // QBE backend - http://c9x.me/compile/
                    hdr_u_len = 0;  // no fiddling necessary!
                    // &ft arg to packExtent will be zero becaue (k != nk_f)
                }
                else {
                    x.offset += delta;
                    x.size   -= delta;
                }
            }
            // compressWithFilters() always assumes a "loader", so would
            // throw NotCompressible for small .data Extents, which PowerPC
            // sometimes marks as PF_X anyway.  So filter only first segment.
            if (k == nk_f || !is_shlib) {
                packExtent(x,
                    (k==nk_f ? &ft : nullptr ), fo, hdr_u_len);
            }
            else {
                total_in += x.size;
            }
        }
        else {
                total_in += x.size;
        }
        hdr_u_len = 0;
        ++nx;
    }
    sz_pack2a = fpad4(fo);  // MATCH01

    // Accounting only; ::pack3 will do the compression and output
    for (k = 0; k < e_phnum; ++k) {
        total_in += find_LOAD_gap(phdri, k, e_phnum);
    }

    if (total_in != (u32_t)file_size)
        throwEOFException();

    return 0;  // omit end-of-compression bhdr for now
}