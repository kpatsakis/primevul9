mb_regex_groups_iter(const OnigUChar* name, const OnigUChar* name_end, int ngroup_num, int* group_nums, regex_t* reg, void* parg)
{
	mb_regex_groups_iter_args *args = (mb_regex_groups_iter_args *) parg;
	int gn, beg, end;

	/*
	 * In case of duplicate groups, keep only the last succeeding one
	 * to be consistent with preg_match with the PCRE_DUPNAMES option.
	 */
	gn = onig_name_to_backref_number(reg, name, name_end, args->region);
	beg = args->region->beg[gn];
	end = args->region->end[gn];
	if (beg >= 0 && beg < end && end <= args->search_len) {
		add_assoc_stringl_ex(args->groups, (char *)name, name_end - name, &args->search_str[beg], end - beg);
	} else {
		add_assoc_bool_ex(args->groups, (char *)name, name_end - name, 0);
	}

	return 0;
}