static struct SYMBOL *get_global_def(struct SYMBOL *s)
{
	struct SYMBOL *s2;

	for (;;) {
		s2 = s->abc_next;
		if (!s2)
			break;
		switch (s2->abc_type) {
		case ABC_T_INFO:
			switch (s2->text[0]) {
			case 'K':
				s = s2;
				s->state = ABC_S_HEAD;
				get_key(s);
				continue;
			case 'I':
			case 'M':
			case 'Q':
				s = s2;
				s->state = ABC_S_HEAD;
				s = get_info(s);
				continue;
			}
			break;
		case ABC_T_PSCOM:
			if (!check_header(s2))
				break;
			s = s2;
			s->state = ABC_S_HEAD;
			s = process_pscomment(s);
			continue;
		}
		break;
	}
	set_global_def();
	return s;
}