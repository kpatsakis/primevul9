PackLinuxElf64::PackLinuxElf64(InputFile *f)
    : super(f), phdri(nullptr), shdri(nullptr),
    gnu_stack(nullptr),
    page_mask(~0ull<<lg2_page),
    dynseg(nullptr), hashtab(nullptr), gashtab(nullptr), dynsym(nullptr),
    jni_onload_sym(nullptr),
    sec_strndx(nullptr), sec_dynsym(nullptr), sec_dynstr(nullptr)
    , symnum_end(0)
{
    memset(&ehdri, 0, sizeof(ehdri));
    if (f) {
        f->seek(0, SEEK_SET);
        f->readx(&ehdri, sizeof(ehdri));
    }
}