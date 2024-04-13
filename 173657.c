static void get_bar(struct SYMBOL *s)
{
	int bar_type;
	struct SYMBOL *s2;

	if (s->u.bar.repeat_bar
	 && curvoice->norepbra
	 && !curvoice->second)
		s->sflags |= S_NOREPBRA;
	if (curvoice->auto_len)
		adjust_dur(s);

	bar_type = s->u.bar.type;
	s2 = curvoice->last_sym;
	if (s2 && s2->type == SPACE) {
		s2->time--;		// keep the space at the right place
	} else if (s2 && s2->type == BAR) {

		/* remove the invisible repeat bars when no shift is needed */
		if (bar_type == B_OBRA
		 && !s2->text
		 && (curvoice == &voice_tb[parsys->top_voice]
		  || (parsys->staff[curvoice->staff - 1].flags & STOP_BAR)
		  || (s->sflags & S_NOREPBRA))) {
			s2->text = s->text;
			s2->u.bar.repeat_bar = s->u.bar.repeat_bar;
			s2->flags |= s->flags & (ABC_F_RBSTART | ABC_F_RBSTOP);
			s2->sflags |= s->sflags
					& (S_NOREPBRA | S_RBSTART | S_RBSTOP);
			s = s2;
			goto gch_build;
		}

		/* merge back-to-back repeat bars */
		if (bar_type == B_LREP && !s->text) {
			if (s2->u.bar.type == B_RREP) {
				s2->u.bar.type = B_DREP;
				s2->flags |= ABC_F_RBSTOP;
				s2->sflags |= S_RBSTOP;
				return;
			}
			if (s2->u.bar.type == B_DOUBLE) {
				s2->u.bar.type = (B_SINGLE << 8) | B_LREP;
				s2->flags |= ABC_F_RBSTOP;
				s2->sflags |= S_RBSTOP;
				return;
			}
		}
	}

	/* link the bar in the voice */
	/* the bar must appear before a key signature */
	if (s2 && s2->type == KEYSIG
	 && (!s2->prev || s2->prev->type != BAR)) {
		curvoice->last_sym = s2->prev;
		if (!curvoice->last_sym)
			curvoice->sym = NULL;
		sym_link(s, BAR);
		s->next = s2;
		s2->prev = s;
		curvoice->last_sym = s2;
	} else {
		sym_link(s, BAR);
	}
	s->staff = curvoice->staff;	/* original staff */

	/* set some flags */
	switch (bar_type) {
	case B_OBRA:
	case (B_OBRA << 4) + B_CBRA:
		s->flags |= ABC_F_INVIS;
		break;
	case (B_COL << 8) + (B_BAR << 4) + B_COL:
	case (B_COL << 12) + (B_BAR << 8) + (B_BAR << 4) + B_COL:
		bar_type = (B_COL << 4) + B_COL;	/* :|: and :||: -> :: */
		s->u.bar.type = bar_type;
		break;
	case (B_BAR << 4) + B_BAR:
		if (!cfmt.rbdbstop)
			break;
	case (B_OBRA << 4) + B_BAR:
	case (B_BAR << 4) + B_CBRA:
		s->flags |= ABC_F_RBSTOP;
		s->sflags |= S_RBSTOP;
		break;
	}

	if (s->u.bar.dc.n > 0)
		deco_cnv(&s->u.bar.dc, s, NULL); /* convert the decorations */

	/* build the gch */
gch_build:
	if (s->text) {
		if (!s->u.bar.repeat_bar) {
			gch_build(s);	/* build the guitar chords */
		} else {
			s->gch = getarena(sizeof *s->gch * 2);
			memset(s->gch, 0, sizeof *s->gch * 2);
			s->gch->type = 'r';
			s->gch->font = REPEATFONT;
			str_font(REPEATFONT);
			s->gch->w = tex_str(s->text);
			s->gch->x = 4 + 4;
		}
	}
}