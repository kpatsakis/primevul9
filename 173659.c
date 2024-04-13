static char *tune_header_rebuild(struct SYMBOL *s)
{
	struct SYMBOL *s2;
	char *header, *p;
	int len;

	len = 0;
	s2 = s;
	for (;;) {
		if (s2->abc_type == ABC_T_INFO) {
			len += strlen(s2->text) + 1;
			if (s2->text[0] == 'K')
				break;
		}
		s2 = s2->abc_next;
	}
	header = malloc(len + 1);
	p = header;
	for (;;) {
		if (s->abc_type == ABC_T_INFO) {
			strcpy(p, s->text);
			p += strlen(p);
			*p++ = '\n';
			if (s->text[0] == 'K')
				break;
		}
		s = s->abc_next;
	}
	*p++ = '\0';
	return header;
}