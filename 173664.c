static void parse_staves(struct SYMBOL *s,
			struct staff_s *staves)
{
	char *p;
	int voice, flags_st, brace, bracket, parenth, err;
	short flags;
	struct staff_s *p_staff;

	/* define the voices */
	err = 0;
	flags = 0;
	brace = bracket = parenth = 0;
	flags_st = 0;
	voice = 0;
	p = s->text + 7;
	while (*p != '\0' && !isspace((unsigned char) *p))
		p++;
	while (*p != '\0') {
		switch (*p) {
		case ' ':
		case '\t':
			break;
		case '[':
			if (parenth || brace + bracket >= 2) {
				error(1, s, "Misplaced '[' in %%%%staves");
				err = 1;
				break;
			}
			if (brace + bracket == 0)
				flags |= OPEN_BRACKET;
			else
				flags |= OPEN_BRACKET2;
			bracket++;
			flags_st <<= 8;
			flags_st |= OPEN_BRACKET;
			break;
		case '{':
			if (parenth || brace || bracket >= 2) {
				error(1, s, "Misplaced '{' in %%%%staves");
				err = 1;
				break;
			}
			if (bracket == 0)
				flags |= OPEN_BRACE;
			else
				flags |= OPEN_BRACE2;
			brace++;
			flags_st <<= 8;
			flags_st |= OPEN_BRACE;
			break;
		case '(':
			if (parenth) {
				error(1, s, "Misplaced '(' in %%%%staves");
				err = 1;
				break;
			}
			flags |= OPEN_PARENTH;
			parenth++;
			flags_st <<= 8;
			flags_st |= OPEN_PARENTH;
			break;
		case '*':
			if (brace && !parenth && !(flags & (OPEN_BRACE | OPEN_BRACE2)))
				flags |= FL_VOICE;
			break;
		case '+':
			flags |= MASTER_VOICE;
			break;
		default:
			if (!isalnum((unsigned char) *p) && *p != '_') {
				error(1, s, "Bad voice ID in %%%%staves");
				err = 1;
				break;
			}
			if (voice >= MAXVOICE) {
				error(1, s, "Too many voices in %%%%staves");
				err = 1;
				break;
			}
			{
				int i, v;
				char sep, *q;

				q = p;
				while (isalnum((unsigned char) *p) || *p == '_')
					p++;
				sep = *p;
				*p = '\0';

				/* search the voice in the voice table */
				v = -1;
				for (i = 0; i < MAXVOICE; i++) {
					if (strcmp(q, voice_tb[i].id) == 0) {
						v = i;
						break;
					}
				}
				if (v < 0) {
					error(1, s,
						"Voice '%s' of %%%%staves has no symbol",
						q);
					err = 1;
//					break;
					p_staff = staves;
				} else {
					p_staff = staves + voice++;
					p_staff->voice = v;
				}
				*p = sep;
			}
			for ( ; *p != '\0'; p++) {
				switch (*p) {
				case ' ':
				case '\t':
					continue;
				case ']':
					if (!(flags_st & OPEN_BRACKET)) {
						error(1, s,
							"Misplaced ']' in %%%%staves");
						err = 1;
						break;
					}
					bracket--;
					if (brace + bracket == 0)
						flags |= CLOSE_BRACKET;
					else
						flags |= CLOSE_BRACKET2;
					flags_st >>= 8;
					continue;
				case '}':
					if (!(flags_st & OPEN_BRACE)) {
						error(1, s,
							"Misplaced '}' in %%%%staves");
						err = 1;
						break;
					}
					brace--;
					if (bracket == 0)
						flags |= CLOSE_BRACE;
					else
						flags |= CLOSE_BRACE2;
					flags &= ~FL_VOICE;
					flags_st >>= 8;
					continue;
				case ')':
					if (!(flags_st & OPEN_PARENTH)) {
						error(1, s,
							"Misplaced ')' in %%%%staves");
						err = 1;
						break;
					}
					parenth--;
					flags |= CLOSE_PARENTH;
					flags_st >>= 8;
					continue;
				case '|':
					flags |= STOP_BAR;
					continue;
				}
				break;
			}
			p_staff->flags = flags;
			flags = 0;
			if (*p == '\0')
				break;
			continue;
		}
		if (*p == '\0')
			break;
		p++;
	}
	if (flags_st != 0) {
		error(1, s, "'}', ')' or ']' missing in %%%%staves");
		err = 1;
	}
	if (err) {
		int i;

		for (i = 0; i < voice; i++)
			staves[i].flags = 0;
	}
	if (voice < MAXVOICE)
		staves[voice].voice = -1;
}