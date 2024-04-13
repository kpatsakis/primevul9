static int anal_fcn_add_bb(RCore *core, const char *input) {
	// fcn_addr bb_addr bb_size [jump] [fail]
	char *ptr;
	const char *ptr2 = NULL;
	ut64 fcnaddr = -1LL, addr = -1LL;
	ut64 size = 0LL;
	ut64 jump = UT64_MAX;
	ut64 fail = UT64_MAX;
	RAnalFunction *fcn = NULL;
	RAnalDiff *diff = NULL;

	while (*input == ' ') input++;
	ptr = strdup (input);

	switch (r_str_word_set0 (ptr)) {
	case 6:
		ptr2 = r_str_word_get0 (ptr, 6);
		if (!(diff = r_anal_diff_new ())) {
			eprintf ("error: Cannot init RAnalDiff\n");
			free (ptr);
			return false;
		}
		if (ptr2[0] == 'm') {
			diff->type = R_ANAL_DIFF_TYPE_MATCH;
		} else if (ptr2[0] == 'u') {
			diff->type = R_ANAL_DIFF_TYPE_UNMATCH;
		}
	case 5: // get fail
		fail = r_num_math (core->num, r_str_word_get0 (ptr, 4));
	case 4: // get jump
		jump = r_num_math (core->num, r_str_word_get0 (ptr, 3));
	case 3: // get size
		size = r_num_math (core->num, r_str_word_get0 (ptr, 2));
	case 2: // get addr
		addr = r_num_math (core->num, r_str_word_get0 (ptr, 1));
	case 1: // get fcnaddr
		fcnaddr = r_num_math (core->num, r_str_word_get0 (ptr, 0));
	}
	fcn = r_anal_get_function_at (core->anal, fcnaddr);
	if (fcn) {
		if (!r_anal_fcn_add_bb (core->anal, fcn, addr, size, jump, fail, diff))
		//if (!r_anal_fcn_add_bb_raw (core->anal, fcn, addr, size, jump, fail, type, diff))
		{
			eprintf ("afb+: Cannot add basic block at 0x%08"PFMT64x"\n", addr);
		}
	} else {
		eprintf ("afb+ Cannot find function at 0x%" PFMT64x " from 0x%08"PFMT64x" -> 0x%08"PFMT64x"\n",
				fcnaddr, addr, jump);
	}
	r_anal_diff_free (diff);
	free (ptr);
	return true;
}