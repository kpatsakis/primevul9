PackLinuxElf::addStubEntrySections(Filter const *)
{
    addLoader("ELFMAINX", nullptr);
    if (hasLoaderSection("ELFMAINXu")) {
            // brk() trouble if static
        addLoader("ELFMAINXu", nullptr);
    }
   //addLoader(getDecompressorSections(), nullptr);
    addLoader(
        ( M_IS_NRV2E(ph.method) ? "NRV_HEAD,NRV2E,NRV_TAIL"
        : M_IS_NRV2D(ph.method) ? "NRV_HEAD,NRV2D,NRV_TAIL"
        : M_IS_NRV2B(ph.method) ? "NRV_HEAD,NRV2B,NRV_TAIL"
        : M_IS_LZMA(ph.method)  ? "LZMA_ELF00,LZMA_DEC20,LZMA_DEC30"
        : nullptr), nullptr);
    if (hasLoaderSection("CFLUSH"))
        addLoader("CFLUSH");
    addLoader("ELFMAINY,IDENTSTR", nullptr);
    if (hasLoaderSection("ELFMAINZe")) { // ppc64 big-endian only
        addLoader("ELFMAINZe", nullptr);
    }
    addLoader("+40,ELFMAINZ", nullptr);
    if (hasLoaderSection("ANDMAJNZ")) { // Android trouble with args to DT_INIT
        if (opt->o_unix.android_shlib) {
            addLoader("ANDMAJNZ", nullptr);  // constant PAGE_SIZE
        }
        else {
            addLoader("ELFMAJNZ", nullptr);  // PAGE_SIZE from AT_PAGESZ
        }
        addLoader("ELFMAKNZ", nullptr);
    }
    if (hasLoaderSection("ELFMAINZu")) {
        addLoader("ELFMAINZu", nullptr);
    }
    addLoader("FOLDEXEC", nullptr);
}