PackLinuxElf::PackLinuxElf(InputFile *f)
    : super(f), e_phnum(0), dynstr(nullptr),
    sz_phdrs(0), sz_elf_hdrs(0), sz_pack2(0), sz_pack2a(0),
    lg2_page(12), page_size(1u<<lg2_page), is_pie(0),
    xct_off(0), xct_va(0), jni_onload_va(0),
    user_init_va(0), user_init_off(0),
    e_machine(0), ei_class(0), ei_data(0), ei_osabi(0), osabi_note(nullptr),
    shstrtab(nullptr),
    o_elf_shnum(0)
{
    memset(dt_table, 0, sizeof(dt_table));
}