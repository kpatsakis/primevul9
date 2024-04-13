static struct SYMBOL *get_lyric(struct SYMBOL *s)
{
	struct SYMBOL *s1, *s2;
	char word[128], *p, *q;
	int ln, cont;
	struct FONTSPEC *f;

	curvoice->have_ly = curvoice->posit.voc != SL_HIDDEN;

	if (curvoice->ignore) {
		for (;;) {
			if (!s->abc_next)
				return s;
			switch (s->abc_next->abc_type) {
			case ABC_T_PSCOM:
				s = process_pscomment(s->abc_next);
				continue;
			case ABC_T_INFO:
				if (s->abc_next->text[0] == 'w'
				 || s->abc_next->text[0] == '+') {
					s = s->abc_next;
					continue;
				}
				break;
			}
			return s;
		}
	}

	f = &cfmt.font_tb[cfmt.vof];
	str_font(cfmt.vof);			/* (for tex_str) */

	/* treat all w: lines */
	cont = 0;
	ln = -1;
	s2 = s1 = NULL;				// have gcc happy
	for (;;) {
		if (!cont) {
			if (ln >= MAXLY- 1) {
				error(1, s, "Too many lyric lines");
				ln--;
			}
			ln++;
			s2 = s1;
			s1 = curvoice->lyric_start;
			if (!s1)
				s1 = curvoice->sym;
			else
				s1 = s1->next;
			if (!s1) {
				error(1, s, "w: without music");
				return s;
			}
		} else {
			cont = 0;
		}

		/* scan the lyric line */
		p = &s->text[2];
		while (*p != '\0') {
			while (isspace((unsigned char) *p))
				p++;
			if (*p == '\0')
				break;
			if (*p == '\\' && p[1] == '\0') {
				cont = 1;
				break;
			}
			switch (*p) {
			case '|':
				while (s1 && s1->type != BAR) {
					s2 = s1;
					s1 = s1->next;
				}
				if (!s1) {
					error(1, s2,
						"Not enough bar lines for lyric line");
					goto ly_next;
				}
				s2 = s1;
				s1 = s1->next;
				p++;
				continue;
			case '-':
				word[0] = LY_HYPH;
				word[1] = '\0';
				p++;
				break;
			case '_':
				word[0] = LY_UNDER;
				word[1] = '\0';
				p++;
				break;
			case '*':
				word[0] = '\0';
				p++;
				break;
			default:
				q = word;
				for (;;) {
					unsigned char c;

					c = *p;
					switch (c) {
					case '\0':
					case ' ':
					case '\t':
					case '_':
					case '*':
					case '|':
						break;
					case '~':
						c = ' ';
						goto addch;
					case '-':
						c = LY_HYPH;
						goto addch;
					case '\\':
						if (p[1] == '\0')
							break;
						switch (p[1]) {
						case '~':
						case '_':
						case '*':
						case '|':
						case '-':
						case ' ':
						case '\\':
							c = *++p;
							break;
						}
						/* fall thru */
					default:
					addch:
						if (q < &word[sizeof word - 1])
							*q++ = c;
						p++;
						if (c == LY_HYPH)
							break;
						continue;
					}
					break;
				}
				*q = '\0';
				break;
			}

			/* store the word in the next note */
			if (s1) {				/* for error */
				s2 = s1;
				s1 = next_lyric_note(s1);
			}
			if (!s1) {
				if (!s2)
					s2 = s;
				error(1, s2, "Too many words in lyric line");
				goto ly_next;
			}
			if (word[0] != '\0'
			 && s1->posit.voc != SL_HIDDEN) {
				struct lyl *lyl;
				float w;

				if (!s1->ly) {
					s1->ly = (struct lyrics *) getarena(sizeof (struct lyrics));
					memset(s1->ly, 0, sizeof (struct lyrics));
				}

				/* handle the font change at start of text */
				q = word;
				if (*q == '$' && isdigit((unsigned char) q[1])
				 && (unsigned) (q[1] - '0') < FONT_UMAX) {
					int ft;

					ft = q[1] - '0';
					if (ft == 0)
						ft = cfmt.vof;
					f = &cfmt.font_tb[ft];
					str_font(ft);
					q += 2;
				}
				w = tex_str(q);
				q = tex_buf;
				lyl = (struct lyl *) getarena(sizeof *s1->ly->lyl[0]
							- sizeof s1->ly->lyl[0]->t
							+ strlen(q) + 1);
				s1->ly->lyl[ln] = lyl;
				lyl->f = f;
				lyl->w = w;
				strcpy(lyl->t, q);

				/* handle the font changes inside the text */
				while (*q != '\0') {
					if (*q == '$' && isdigit((unsigned char) q[1])
					 && (unsigned) (q[1] - '0') < FONT_UMAX) {
						int ft;

						q++;
						ft = *q - '0';
						if (ft == 0)
							ft = cfmt.vof;
						f = &cfmt.font_tb[ft];
						str_font(ft);
					}
					q++;
				}
			}
			s2 = s1;
			s1 = s1->next;
		}

		/* loop if more lyrics */
ly_next:
		for (;;) {
			if (!s->abc_next)
				goto ly_upd;
			switch (s->abc_next->abc_type) {
			case ABC_T_PSCOM:
				s = process_pscomment(s->abc_next);
				f = &cfmt.font_tb[cfmt.vof];	/* may have changed */
				str_font(cfmt.vof);
				continue;
			case ABC_T_INFO:
				if (s->abc_next->text[0] != 'w'
				 && s->abc_next->text[0] != '+')
					goto ly_upd;
				s = s->abc_next;
				if (s->text[0] == '+')
					cont = 1;
				if (!cont) {
					s1 = next_lyric_note(s1);
					if (s1) {
						error(1, s1,
							"Not enough words for lyric line");
					}
				}
				break;			/* more lyric */
			default:
				goto ly_upd;
			}
			break;
		}
	}

	/* the next lyrics will go into the next notes */
ly_upd:
//fixme: no error with abc-2.1
	if (next_lyric_note(s1))
		error(0, s1, "Not enough words for lyric line");
	// fill the w: with 'blank syllabes'
	curvoice->lyric_start = curvoice->last_sym;
	return s;
}