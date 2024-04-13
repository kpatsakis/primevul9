S_debug_studydata(pTHX_ const char *where, scan_data_t *data,
                    U32 depth, int is_inf)
{
    GET_RE_DEBUG_FLAGS_DECL;

    DEBUG_OPTIMISE_MORE_r({
        if (!data)
            return;
        Perl_re_indentf(aTHX_  "%s: Pos:%" IVdf "/%" IVdf " Flags: 0x%" UVXf,
            depth,
            where,
            (IV)data->pos_min,
            (IV)data->pos_delta,
            (UV)data->flags
        );

        S_debug_show_study_flags(aTHX_ data->flags," [","]");

        Perl_re_printf( aTHX_
            " Whilem_c: %" IVdf " Lcp: %" IVdf " %s",
            (IV)data->whilem_c,
            (IV)(data->last_closep ? *((data)->last_closep) : -1),
            is_inf ? "INF " : ""
        );

        if (data->last_found) {
            int i;
            Perl_re_printf(aTHX_
                "Last:'%s' %" IVdf ":%" IVdf "/%" IVdf,
                    SvPVX_const(data->last_found),
                    (IV)data->last_end,
                    (IV)data->last_start_min,
                    (IV)data->last_start_max
            );

            for (i = 0; i < 2; i++) {
                Perl_re_printf(aTHX_
                    " %s%s: '%s' @ %" IVdf "/%" IVdf,
                    data->cur_is_floating == i ? "*" : "",
                    i ? "Float" : "Fixed",
                    SvPVX_const(data->substrs[i].str),
                    (IV)data->substrs[i].min_offset,
                    (IV)data->substrs[i].max_offset
                );
                S_debug_show_study_flags(aTHX_ data->substrs[i].flags," [","]");
            }
        }

        Perl_re_printf( aTHX_ "\n");
    });
}