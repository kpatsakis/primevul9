static void gch_build(struct SYMBOL *s)
{
	struct gch *gch;
	char *p, *q, antype, sep;
	float w, h_ann, h_gch, y_above, y_below, y_left, y_right;
	float xspc;
	int l, ix, box, gch_place;

	if (s->posit.gch == SL_HIDDEN)
		return;
	s->gch = getarena(sizeof *s->gch * MAXGCH);
	memset(s->gch, 0, sizeof *s->gch * MAXGCH);

	if (cfmt.capo)
		gch_capo(s);
	if (curvoice->transpose != 0)
		gch_transpose(s);

	/* split the guitar chords / annotations
	 * and initialize their vertical offsets */
	gch_place = s->posit.gch == SL_BELOW ? -1 : 1;
	h_gch = cfmt.font_tb[cfmt.gcf].size;
	h_ann = cfmt.font_tb[cfmt.anf].size;
	y_above = y_below = y_left = y_right = 0;
	box = cfmt.gchordbox;
	p = s->text;
	gch = s->gch;
	sep = '\n';
	antype = 'g';			/* (compiler warning) */
	for (;;) {
		if (sep != 'n' && strchr("^_<>@", *p)) {
			gch->font = cfmt.anf;
			antype = *p++;
			if (antype == '@') {
				int n;
				float xo, yo;

				if (sscanf(p, "%f,%f%n", &xo, &yo, &n) != 2) {
					error(1, s, "Error in annotation \"@\"");
				} else {
					p += n;
					if (*p == ' ')
						p++;
					gch->x = xo;
					gch->y = yo;
				}
			}
		} else if (sep == '\n') {
			gch->font = cfmt.gcf;
			gch->box = box;
			antype = 'g';
		} else {
			gch->font = (gch - 1)->font;
			gch->box = (gch - 1)->box;
		}
		gch->type = antype;
		switch (antype) {
		default:				/* guitar chord */
			if (gch_place < 0)
				break;			/* below */
			y_above += h_gch;
			if (box)
				y_above += 2;
			break;
		case '^':				/* above */
			y_above += h_ann;
			break;
		case '_':				/* below */
			break;
		case '<':				/* left */
			y_left += h_ann * 0.5;
			break;
		case '>':				/* right */
			y_right += h_ann * 0.5;
			break;
		case '@':				/* absolute */
			if (gch->x == 0 && gch->y == 0
			 && gch != s->gch
			 && s->gch->type == '@') {	/* if not 1st line */
				gch->x = (gch - 1)->x;
				gch->y = (gch - 1)->y - h_ann;
			}
			break;
		}
		gch->idx = p - s->text;
		for (;;) {
			switch (*p) {
			default:
				p++;
				continue;
			case '\\':
				p++;
				if (*p == 'n') {
					p[-1] = '\0';
					break;		/* sep = 'n' */
				}
				p++;
				continue;
			case '&':			/* skip "&xxx;" */
				for (;;) {
					switch (*p) {
					default:
						p++;
						continue;
					case ';':
						p++;
					case '\0':
					case '\n':
					case '\\':
						break;
					}
					break;
				}
				continue;
			case '\0':
			case ';':
			case '\n':
				break;
			}
			break;
		}
		sep = *p;
		if (sep == '\0')
			break;
		*p++ = '\0';
		gch++;
		if (gch - s->gch >= MAXGCH) {
			error(1, s, "Too many guitar chords / annotations");
			break;
		}
	}

	/* change the accidentals in the guitar chords */
	for (ix = 0, gch = s->gch; ix < MAXGCH; ix++, gch++) {
		if (gch->type == '\0')
			break;
		if (gch->type != 'g')
			continue;
		p = s->text + gch->idx;
		q = p;
		for (; *p != '\0'; p++) {
			switch (*p) {
			case '#':
			case 'b':
			case '=':
				if (p == q	/* 1st char or after a slash */
				 || (p != q + 1	/* or invert '\' behaviour */
				  && p[-1] == '\\'))
					break;

				/* set the accidentals as unused utf-8 values
				 * (see subs.c) */
				switch (*p) {
				case '#':
					*p = 0x01;
					break;
				case 'b':
					*p = 0x02;
					break;
				default:
/*				case '=': */
					*p = 0x03;
					break;
				}
				if (p[-1] == '\\') {
					p--;
					l = strlen(p);
					memmove(p, p + 1, l);
				}
				break;
			case ' ':
			case '/':
				q = p + 1;
				break;
			}
		}
	}

	/* set the offsets and widths */
/*fixme:utf8*/
	for (ix = 0, gch = s->gch; ix < MAXGCH; ix++, gch++) {
		if (gch->type == '\0')
			break;
		if (gch->type == '@')
			continue;		/* no width */
		p = s->text + gch->idx;
		str_font(gch->font);
		w = tex_str(p);
		gch->w = w; // + 4;
		switch (gch->type) {
		case '_':			/* below */
			xspc = w * GCHPRE;
			if (xspc > 8)
				xspc = 8;
			gch->x = -xspc;
			y_below -= h_ann;
			gch->y = y_below;
			break;
		case '^':			/* above */
			xspc = w * GCHPRE;
			if (xspc > 8)
				xspc = 8;
			gch->x = -xspc;
			y_above -= h_ann;
			gch->y = y_above;
			break;
		default:			/* guitar chord */
			xspc = w * GCHPRE;
			if (xspc > 8)
				xspc = 8;
			gch->x = -xspc;
			if (gch_place < 0) {	/* below */
				y_below -= h_gch;
				gch->y = y_below;
				if (box) {
					y_below -= 2;
					gch->y -= 1;
				}
			} else {
				y_above -= h_gch;
				gch->y = y_above;
				if (box) {
					y_above -= 2;
					gch->y -= 1;
				}
			}
			break;
		case '<':		/* left */
			gch->x = -(w + 6);
			y_left -= h_ann;
			gch->y = y_left;
			break;
		case '>':		/* right */
			gch->x = 6;
			y_right -= h_ann;
			gch->y = y_right;
			break;
		}
	}
}