static void tune_filter(struct SYMBOL *s)
{
	struct tune_opt_s *opt;
	struct SYMBOL *s1, *s2;
	regex_t r;
	char *header, *p;
	int ret;

	header = tune_header_rebuild(s);
	for (opt = tune_opts; opt; opt = opt->next) {
		struct SYMBOL *last_staves;

		p = &opt->s->text[2 + 5];	/* "%%tune RE" */
		while (isspace((unsigned char) *p))
			p++;

		ret = regcomp(&r, p, REG_EXTENDED | REG_NEWLINE | REG_NOSUB);
		if (ret)
			continue;
		ret = regexec(&r, header, 0, NULL, 0);
		regfree(&r);
		if (ret)
			continue;

		/* apply the options */
		cur_tune_opts = opt;
		last_staves = s->abc_next;
		for (s1 = opt->s->next; s1; s1 = s1->next) {

			/* replace the next %%staves/%%score */
			if (s1->abc_type == ABC_T_PSCOM
			 && (strncmp(&s1->text[2], "staves", 6) == 0
			  || strncmp(&s1->text[2], "score", 5) == 0)) {
				while (last_staves) {
					if (last_staves->abc_type == ABC_T_PSCOM
					 && (strncmp(&last_staves->text[2],
								"staves", 6) == 0
					  || strncmp(&last_staves->text[2],
								 "score", 5) == 0)) {
						last_staves->text = s1->text;
						last_staves = last_staves->abc_next;
						break;
					}
					last_staves = last_staves->abc_next;
				}
				continue;
			}
			s2 = (struct SYMBOL *) getarena(sizeof *s2);
			memcpy(s2, s1, sizeof *s2);
			process_pscomment(s2);
		}
		cur_tune_opts = NULL;
		tune_voice_opts = opt->voice_opts;	// for %%voice
//fixme: what if many %%tune's with %%voice inside?
	}
	free(header);
}