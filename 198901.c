static void filter_build_regex(struct filter_pred *pred)
{
	struct regex *r = &pred->regex;
	char *search;
	enum regex_type type = MATCH_FULL;

	if (pred->op == OP_GLOB) {
		type = filter_parse_regex(r->pattern, r->len, &search, &pred->not);
		r->len = strlen(search);
		memmove(r->pattern, search, r->len+1);
	}

	switch (type) {
	/* MATCH_INDEX should not happen, but if it does, match full */
	case MATCH_INDEX:
	case MATCH_FULL:
		r->match = regex_match_full;
		break;
	case MATCH_FRONT_ONLY:
		r->match = regex_match_front;
		break;
	case MATCH_MIDDLE_ONLY:
		r->match = regex_match_middle;
		break;
	case MATCH_END_ONLY:
		r->match = regex_match_end;
		break;
	case MATCH_GLOB:
		r->match = regex_match_glob;
		break;
	}
}