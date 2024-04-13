PackLinuxElf32::buildLinuxLoader(
    upx_byte const *const proto,
    unsigned        const szproto,
    upx_byte const *const fold,
    unsigned        const szfold,
    Filter const *ft
)
{
    initLoader(proto, szproto);

  if (0 < szfold) {
    struct b_info h; memset(&h, 0, sizeof(h));
    unsigned fold_hdrlen = 0;
    cprElfHdr1 const *const hf = (cprElfHdr1 const *)fold;
    fold_hdrlen = umax(0x80, sizeof(hf->ehdr) +
        get_te16(&hf->ehdr.e_phentsize) * get_te16(&hf->ehdr.e_phnum) +
            sizeof(l_info) );
    h.sz_unc = ((szfold < fold_hdrlen) ? 0 : (szfold - fold_hdrlen));
    h.b_method = (unsigned char) ph.method;
    h.b_ftid = (unsigned char) ph.filter;
    h.b_cto8 = (unsigned char) ph.filter_cto;
    unsigned char const *const uncLoader = fold_hdrlen + fold;

    MemBuffer mb_cprLoader;
    mb_cprLoader.allocForCompression(h.sz_unc + (0==h.sz_unc));
    h.sz_cpr = mb_cprLoader.getSize();
    unsigned char *const cprLoader = (unsigned char *)mb_cprLoader;
    {
    unsigned h_sz_cpr = h.sz_cpr;
    int r = upx_compress(uncLoader, h.sz_unc, sizeof(h) + cprLoader, &h_sz_cpr,
        nullptr, ph.method, 10, nullptr, nullptr );
    h.sz_cpr = h_sz_cpr;
    if (r != UPX_E_OK || h.sz_cpr >= h.sz_unc)
        throwInternalError("loader compression failed");
    }
#if 0  //{  debugging only
    if (M_IS_LZMA(ph.method)) {
        ucl_uint tmp_len = h.sz_unc;  // LZMA uses this as EOF
        MemBuffer mb_tmp(tmp_len);
        unsigned char *tmp = (unsigned char *)mb_tmp;
        memset(tmp, 0, tmp_len);
        int r = upx_decompress(sizeof(h) + cprLoader, h.sz_cpr, tmp, &tmp_len, h.b_method, nullptr);
        if (r == UPX_E_OUT_OF_MEMORY)
            throwOutOfMemoryException();
        printf("\n%d %d: %d %d %d\n", h.b_method, r, h.sz_cpr, h.sz_unc, tmp_len);
        for (unsigned j=0; j < h.sz_unc; ++j) if (tmp[j]!=uncLoader[j]) {
            printf("%d: %x %x\n", j, tmp[j], uncLoader[j]);
        }
    }
#endif  //}
    unsigned const sz_cpr = h.sz_cpr;
    set_te32(&h.sz_cpr, h.sz_cpr);
    set_te32(&h.sz_unc, h.sz_unc);
    memcpy(cprLoader, &h, sizeof(h));

    // This adds the definition to the "library", to be used later.
    linker->addSection("FOLDEXEC", cprLoader, sizeof(h) + sz_cpr, 0);
  }
  else {
    linker->addSection("FOLDEXEC", "", 0, 0);
  }

    addStubEntrySections(ft);

    if (0==xct_off)
        defineSymbols(ft);  // main program only, not for shared lib
    relocateLoader();
}