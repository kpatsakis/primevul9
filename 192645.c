S_scan_commit(pTHX_ const RExC_state_t *pRExC_state, scan_data_t *data,
                    SSize_t *minlenp, int is_inf)
{
    const STRLEN l = CHR_SVLEN(data->last_found);
    SV * const longest_sv = data->substrs[data->cur_is_floating].str;
    const STRLEN old_l = CHR_SVLEN(longest_sv);
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_SCAN_COMMIT;

    if ((l >= old_l) && ((l > old_l) || (data->flags & SF_BEFORE_EOL))) {
        const U8 i = data->cur_is_floating;
	SvSetMagicSV(longest_sv, data->last_found);
        data->substrs[i].min_offset = l ? data->last_start_min : data->pos_min;

	if (!i) /* fixed */
	    data->substrs[0].max_offset = data->substrs[0].min_offset;
	else { /* float */
	    data->substrs[1].max_offset = (l
                          ? data->last_start_max
                          : (data->pos_delta > SSize_t_MAX - data->pos_min
					 ? SSize_t_MAX
					 : data->pos_min + data->pos_delta));
	    if (is_inf
		 || (STRLEN)data->substrs[1].max_offset > (STRLEN)SSize_t_MAX)
		data->substrs[1].max_offset = SSize_t_MAX;
        }

        if (data->flags & SF_BEFORE_EOL)
            data->substrs[i].flags |= (data->flags & SF_BEFORE_EOL);
        else
            data->substrs[i].flags &= ~SF_BEFORE_EOL;
        data->substrs[i].minlenp = minlenp;
        data->substrs[i].lookbehind = 0;
    }

    SvCUR_set(data->last_found, 0);
    {
	SV * const sv = data->last_found;
	if (SvUTF8(sv) && SvMAGICAL(sv)) {
	    MAGIC * const mg = mg_find(sv, PERL_MAGIC_utf8);
	    if (mg)
		mg->mg_len = 0;
	}
    }
    data->last_end = -1;
    data->flags &= ~SF_BEFORE_EOL;
    DEBUG_STUDYDATA("commit", data, 0, is_inf);
}