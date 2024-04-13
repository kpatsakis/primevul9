generate_form_error(Dwarf_Debug dbg,
    Dwarf_Error *error,
    unsigned form,
    int err_code,
    const char *errname,
    const char *funcname)
{
    dwarfstring m;
    char mbuf[DWARFSTRING_ALLOC_SIZE];
    const char * defaultname = "<unknown form>";

    dwarfstring_constructor_static(&m,mbuf,
        sizeof(mbuf));
    dwarfstring_append(&m,(char *)errname);
    dwarfstring_append(&m,": In function ");
    dwarfstring_append(&m,(char *)funcname);
    dwarfstring_append_printf_u(&m,
        " on seeing form  0x%x ",form);
    dwarf_get_FORM_name(form,&defaultname);
    dwarfstring_append_printf_s(&m,
        " (%s)",(char *)defaultname);
    _dwarf_error_string(dbg,error,err_code,
        dwarfstring_string(&m));
    dwarfstring_destructor(&m);
}