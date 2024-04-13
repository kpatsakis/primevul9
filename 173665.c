static void voice_filter(void)
{
	struct voice_opt_s *opt;
	struct SYMBOL *s;
	regex_t r;
	int pass, ret;
	char *p;

	/* scan the global, then the tune options */
	pass = 0;
	opt = voice_opts;
	for (;;) {
		if (!opt) {
			if (pass != 0)
				break;
			opt = tune_voice_opts;
			if (!opt)
				break;
			pass++;
		}
		p = &opt->s->text[2 + 6];	/* "%%voice RE" */
		while (isspace((unsigned char) *p))
			p++;

		ret = regcomp(&r, p, REG_EXTENDED | REG_NOSUB);
		if (ret)
			goto next_voice;
		ret = regexec(&r, curvoice->id, 0, NULL, 0);
		if (ret && curvoice->nm)
			ret = regexec(&r, curvoice->nm, 0, NULL, 0);
		regfree(&r);
		if (ret)
			goto next_voice;

		/* apply the options */
		for (s = opt->s->next; s; s = s->next) {
			struct SYMBOL *s2;

			s2 = (struct SYMBOL *) getarena(sizeof *s2);
			memcpy(s2, s, sizeof *s2);
			process_pscomment(s2);
		}
next_voice:
		opt = opt->next;
	}
}