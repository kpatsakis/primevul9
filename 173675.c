static void clef_def(struct SYMBOL *s)
{
	char *p;
	int clef, clef_line;
	char str[80];

	clef = -1;
	clef_line = 2;
	p = &s->text[2 + 5];		/* skip %%clef */
	while (isspace((unsigned char) *p))
		p++;

	/* clef name */
	switch (*p) {
	case '\"':			/* user clef name */
		p = get_str(str, p, sizeof str);
		s->u.clef.name = (char *) getarena(strlen(str) + 1);
		strcpy(s->u.clef.name, str);
		clef = TREBLE;
		break;
	case 'G':
		clef = TREBLE;
		p++;
		break;
	case 'F':
		clef = BASS;
		clef_line = 4;
		p++;
		break;
	case 'C':
		clef = ALTO;
		clef_line = 3;
		p++;
		break;
	case 'P':
		clef = PERC;
		p++;
		break;
	case 't':
		if (strncmp(p, "treble", 6) == 0) {
			clef = TREBLE;
			p += 6;
		}
		if (strncmp(p, "tenor", 5) == 0) {
			clef = ALTO;
			clef_line = 4;
			p += 5;
		}
		break;
	case 'a':
		if (strncmp(p, "alto", 4) == 0) {
			clef = ALTO;
			clef_line = 3;
			p += 4;
		} else if (strncmp(p, "auto", 4) == 0) {
			clef = AUTOCLEF;
			s->sflags |= S_CLEF_AUTO;
			p += 4;
		}
		break;
	case 'b':
		if (strncmp(p, "bass", 4) == 0) {
			clef = BASS;
			clef_line = 4;
			p += 4;
		}
		break;
	case 'p':
		if (strncmp(p, "perc", 4) == 0) {
			clef = PERC;
			p += 4;
		}
		break;
	case 'n':
		if (strncmp(p, "none", 4) == 0) {
			clef = TREBLE;
			s->u.clef.invis = 1;
			s->flags |= ABC_F_INVIS;
			p += 4;
		}
		break;
	}
	if (clef < 0) {
		error(1, s, "Unknown clef '%s'", p);
		return;
	}

	/* clef line */
	switch (*p) {
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
		clef_line = *p++ - '0';
		break;
	}

	/* +/-/^/_8 */
	if (p[1] == '8') {
		switch (*p) {
		case '^':
			s->u.clef.transpose = -7;
		case '+':
			s->u.clef.octave = 1;
			break;
		case '_':
			s->u.clef.transpose = 7;
		case '-':
			s->u.clef.octave = -1;
			break;
		}
	}

	/* handle the clef */
	s->abc_type = ABC_T_CLEF;
	s->u.clef.type = clef;
	s->u.clef.line = clef_line;
	get_clef(s);
}