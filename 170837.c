_dwarf_extract_string_offset_via_str_offsets(Dwarf_Debug dbg,
    Dwarf_Small *data_ptr,
    Dwarf_Small *end_data_ptr,
    Dwarf_Half   attrnum UNUSEDARG,
    Dwarf_Half   attrform,
    Dwarf_CU_Context cu_context,
    Dwarf_Unsigned *str_sect_offset_out,
    Dwarf_Error *error)
{
    Dwarf_Unsigned index_to_offset_entry = 0;
    Dwarf_Unsigned offsetintable = 0;
    Dwarf_Unsigned end_offsetintable = 0;
    Dwarf_Unsigned indexoffset = 0;
    Dwarf_Unsigned baseoffset = 0;
    int res = 0;
    int idxres = 0;
    Dwarf_Small *sof_start = 0;
    Dwarf_Unsigned sof_len = 0;
    Dwarf_Small   *sof_end = 0;

    res = _dwarf_load_section(dbg, &dbg->de_debug_str_offsets,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    /*  If this is a dwp we look there, but I suppose
        we could also look for the section in the tied
        file it is not here. */
    sof_start = dbg->de_debug_str_offsets.dss_data;
    sof_len = dbg->de_debug_str_offsets.dss_size;
    sof_end = sof_start+sof_len;
    idxres = dw_read_str_index_val_itself(dbg,
        attrform,data_ptr,end_data_ptr,&index_to_offset_entry,error);
    if ( idxres != DW_DLV_OK) {
        return idxres;
    }

    if (cu_context->cc_str_offsets_base_present) {
        baseoffset = cu_context->cc_str_offsets_base;
    }
    indexoffset = index_to_offset_entry*
        cu_context->cc_length_size;
    baseoffset = cu_context->cc_str_offsets_base;
    if (!baseoffset) {
        if (cu_context->cc_version_stamp ==  DW_CU_VERSION5 ) {
            /*  A base offset of 0 isnormally never correct for
                DWARF5. but some early GNU compilers emitted
                DWARF4 .debug_str_offsets, so lets check
                the first table.  */
            Dwarf_Small * ststart =
                dbg->de_debug_str_offsets.dss_data;
            Dwarf_Small * stend = 0;
            Dwarf_Unsigned  stsize =
                dbg->de_debug_str_offsets.dss_size;
            Dwarf_Unsigned length            = 0;
            Dwarf_Half local_offset_size = 0;
            Dwarf_Half local_extension_size = 0;
            Dwarf_Half version               = 0;
            Dwarf_Half padding               = 0;

            stend = ststart + stsize;
            res = _dwarf_trial_read_dwarf_five_hdr(dbg,
                ststart,stsize,stend,
                &length, &local_offset_size,
                &local_extension_size,
                &version,
                &padding,
                error);
            if (res == DW_DLV_OK) {
                baseoffset = local_extension_size +
                    local_offset_size +
                    2*DWARF_HALF_SIZE;
            } else {
                if (res == DW_DLV_ERROR) {
                    dwarf_dealloc_error(dbg,*error);
                    *error = 0;
                } else {}
            }
        }
    }
    offsetintable = baseoffset +indexoffset;
    end_offsetintable = offsetintable +
        cu_context->cc_str_offsets_offset_size;
    /*  The offsets table is a series of offset-size entries.
        The == case in the test applies when we are at the last table
        entry, so == is not an error, hence only test >
    */
    if (end_offsetintable > dbg->de_debug_str_offsets.dss_size ) {
        dwarfstring m;

        dwarfstring_constructor(&m);
        dwarfstring_append_printf_u(&m,
            "DW_DLE_ATTR_FORM_SIZE_BAD: The end offset of "
            "a .debug_str_offsets table is 0x%x ",
            end_offsetintable);
        dwarfstring_append_printf_u(&m,
            "but the object section is just 0x%x "
            "bytes long",
            dbg->de_debug_str_offsets.dss_size);
        _dwarf_error_string(dbg, error,
            DW_DLE_ATTR_FORM_SIZE_BAD,
            dwarfstring_string(&m));
        dwarfstring_destructor(&m);
        return DW_DLV_ERROR;
    }

    {
        Dwarf_Unsigned offsettostr = baseoffset+offsetintable;

        /* Now read the string offset from the offset table. */
        READ_UNALIGNED_CK(dbg,offsettostr,Dwarf_Unsigned,
            sof_start+ offsetintable,
            cu_context->cc_length_size,error,sof_end);
        *str_sect_offset_out = offsettostr;
    }
    return DW_DLV_OK;
}