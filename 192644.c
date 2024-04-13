S_handle_named_backref(pTHX_ RExC_state_t *pRExC_state,
                             I32 *flagp,
                             char * parse_start,
                             char ch
                      )
{
    regnode *ret;
    char* name_start = RExC_parse;
    U32 num = 0;
    SV *sv_dat = reg_scan_name(pRExC_state, SIZE_ONLY
                                            ? REG_RSN_RETURN_NULL
                                            : REG_RSN_RETURN_DATA);
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_HANDLE_NAMED_BACKREF;

    if (RExC_parse == name_start || *RExC_parse != ch) {
        /* diag_listed_as: Sequence \%s... not terminated in regex; marked by <-- HERE in m/%s/ */
        vFAIL2("Sequence %.3s... not terminated",parse_start);
    }

    if (!SIZE_ONLY) {
        num = add_data( pRExC_state, STR_WITH_LEN("S"));
        RExC_rxi->data->data[num]=(void*)sv_dat;
        SvREFCNT_inc_simple_void(sv_dat);
    }
    RExC_sawback = 1;
    ret = reganode(pRExC_state,
                   ((! FOLD)
                     ? NREF
                     : (ASCII_FOLD_RESTRICTED)
                       ? NREFFA
                       : (AT_LEAST_UNI_SEMANTICS)
                         ? NREFFU
                         : (LOC)
                           ? NREFFL
                           : NREFF),
                    num);
    *flagp |= HASWIDTH;

    Set_Node_Offset(ret, parse_start+1);
    Set_Node_Cur_Length(ret, parse_start);

    nextchar(pRExC_state);
    return ret;
}