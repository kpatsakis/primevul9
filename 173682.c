static void gch_tr1(struct SYMBOL *s, int i, int i2)
{
	char *p = &s->text[i],
		*q = p + 1,
		*new_txt;
	int l, latin;
	int n, a, i1, i3, i4;
	static const char note_names[] = "CDEFGAB";
	static const char *latin_names[7] =
			{ "Do", "Ré", "Mi", "Fa", "Sol", "La", "Si" };
	static const char *acc_name[5] = {"bb", "b", "", "#", "##"};
	static const char note_pit[] = {0, 2, 4, 5, 7, 9, 11};
	static const char pit_note[] = {0, 0, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6};
	static const char pit_acc[] = {2, 3, 2, 1, 2, 2, 3, 2, 1, 2, 1, 2};

	/* main chord */
	latin = 0;
	switch (*p) {
	case 'A':
	case 'B':
		n = *p - 'A' + 5;
		break;
	case 'C':
	case 'E':
	case 'G':
		n = *p - 'C';
		break;
	case 'D':
		if (p[1] == 'o') {
			latin++;
			n = 0;		/* Do */
			break;
		}
		n = 1;
		break;
	case 'F':
		if (p[1] == 'a')
			latin++;	/* Fa */
		n = 3;
		break;
	case 'L':
		latin++;		/* La */
		n = 5;
		break;
	case 'M':
		latin++;		/* Mi */
		n = 2;
		break;
	case 'R':
		latin++;
		if (p[1] != 'e')
			latin++;	/* Ré */
		n = 1;			/* Re */
		break;
	case 'S':
		latin++;
		if (p[1] == 'o') {
			latin++;
			n = 4;		/* Sol */
		} else {
			n = 6;		/* Si */
		}
		break;
	case '/':			// bass only
		latin--;
		break;
	default:
		return;
	}
	q += latin;

	/* allocate a new string */
	new_txt = getarena(strlen(s->text) + 6);
	l = p - s->text;
	memcpy(new_txt, s->text, l);
	s->text = new_txt;
	new_txt += l;
	p = q;

	if (latin >= 0) {			// if some chord
		a = 0;
		while (*p == '#') {
			a++;
			p++;
		}
		while (*p == 'b') {
			a--;
			p++;
		}
		if (*p == '=')
			p++;
		i3 = (note_pit[n] + a + i2 + 12) % 12;
		i4 = pit_note[i3];
		if (latin == 0)
			*new_txt++ = note_names[i4];
		else
			new_txt += sprintf(new_txt, "%s", latin_names[i4]);
		i1 = pit_acc[i3];			// accidental
		new_txt += sprintf(new_txt, "%s", acc_name[i1]);
	}

	/* bass */
	while (*p != '\0' && *p != '\n' && *p != '/')	// skip 'm'/'dim'..
		*new_txt++ = *p++;
	if (*p == '/') {
		*new_txt++ = *p++;
//fixme: latin names not treated
		q = strchr(note_names, *p);
		if (q) {
			p++;
			n = q - note_names;
			if (*p == '#') {
				a = 1;
				p++;
			} else if (*p == 'b') {
				a = -1;
				p++;
			} else {
				a = 0;
			}
			i3 = (note_pit[n] + a + i2 + 12) % 12;
			i4 = pit_note[i3];
			*new_txt++ = note_names[i4];
			i1 = pit_acc[i3];
			new_txt += sprintf(new_txt, "%s", acc_name[i1]);
		}
	}
	strcpy(new_txt, p);
}