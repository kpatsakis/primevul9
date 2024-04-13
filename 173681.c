static void get_note(struct SYMBOL *s)
{
	struct SYMBOL *prev;
	int i, m, delta;

	prev = curvoice->last_sym;
	m = s->nhd;

	/* insert the note/rest in the voice */
	sym_link(s,  s->u.note.notes[0].len != 0 ? NOTEREST : SPACE);
	if (!(s->flags & ABC_F_GRACE))
		curvoice->time += s->dur;

	if (curvoice->octave) {
		delta = curvoice->octave * 7;
		for (i = 0; i <= m; i++) {
			s->u.note.notes[i].pit += delta;
			s->pits[i] += delta;
		}
	}

	/* convert the decorations
	 * (!beam-accel! and !beam-rall! may change the note duration)
	 * (!8va(! may change ottava)
	 */
	if (s->u.note.dc.n > 0)
		deco_cnv(&s->u.note.dc, s, prev);

	if (curvoice->ottava) {
		delta = curvoice->ottava;
		for (i = 0; i <= m; i++)
			s->pits[i] += delta;
	}
	s->combine = curvoice->combine;
	s->color = curvoice->color;

	if (curvoice->perc)
		s->sflags |= S_PERC;
	else if (s->abc_type == ABC_T_NOTE
	      && curvoice->transpose != 0)
		note_transpose(s);

	if (!(s->flags & ABC_F_GRACE)) {
		switch (curvoice->posit.std) {
		case SL_ABOVE: s->stem = 1; break;
		case SL_BELOW: s->stem = -1; break;
		case SL_HIDDEN: s->flags |= ABC_F_STEMLESS;; break;
		}
	} else {			/* grace note - adjust its duration */
		int div;

		if (curvoice->key.instr != K_HP
		 && curvoice->key.instr != K_Hp
		 && !pipeformat) {
			div = 2;
			if (!prev
			 || !(prev->flags & ABC_F_GRACE)) {
				if (s->flags & ABC_F_GR_END)
					div = 1;	/* one grace note */
			}
		} else {
			div = 4;			/* bagpipe */
		}
		for (i = 0; i <= m; i++)
			s->u.note.notes[i].len /= div;
		s->dur /= div;
		switch (curvoice->posit.gsd) {
		case SL_ABOVE: s->stem = 1; break;
		case SL_BELOW: s->stem = -1; break;
		case SL_HIDDEN:	s->stem = 2; break;	/* opposite */
		}
	}

	s->nohdi1 = s->nohdi2 = -1;

	/* change the figure of whole measure rests */
	if (s->abc_type == ABC_T_REST) {
		if (s->dur == curvoice->wmeasure) {
			if (s->dur < BASE_LEN * 2)
				s->u.note.notes[0].len = BASE_LEN;
			else if (s->dur < BASE_LEN * 4)
				s->u.note.notes[0].len = BASE_LEN * 2;
			else
				s->u.note.notes[0].len = BASE_LEN * 4;
		}
	} else {

		/* sort the notes of the chord by pitch (lowest first) */
		if (!(s->flags & ABC_F_GRACE)
		 && curvoice->map_name)
			set_map(s);
		sort_pitch(s);
	}

	/* get the max head type, number of dots and number of flags */
	if (!curvoice->auto_len || (s->flags & ABC_F_GRACE)) {
		int head, dots, nflags, l;

		if ((l = s->u.note.notes[0].len) != 0) {
			identify_note(s, l, &head, &dots, &nflags);
			s->head = head;
			s->dots = dots;
			s->nflags = nflags;
			for (i = 1; i <= m; i++) {
				if (s->u.note.notes[i].len == l)
					continue;
				identify_note(s, s->u.note.notes[i].len,
						&head, &dots, &nflags);
				if (head > s->head)
					s->head = head;
				if (dots > s->dots)
					s->dots = dots;
				if (nflags > s->nflags)
					s->nflags = nflags;
			}
			if (s->sflags & S_XSTEM)
				s->nflags = 0;		/* word start+end */
		}
	}
	if (s->nflags <= -2)
		s->flags |= ABC_F_STEMLESS;

	if (s->sflags & (S_TREM1 | S_TREM2)) {
		if (s->nflags > 0)
			s->nflags += s->aux;
		else
			s->nflags = s->aux;
		if ((s->sflags & S_TREM2)
		 && (s->sflags & S_BEAM_END)) {		/* if 2nd note - see deco.c */
			prev->head = s->head;
			prev->aux = s->aux;
			prev->nflags = s->nflags;
			prev->flags |= (s->flags & ABC_F_STEMLESS);
		}
	}

	for (i = 0; i <= m; i++) {
		if (s->u.note.notes[i].sl1 != 0)
			s->sflags |= S_SL1;
		if (s->u.note.notes[i].sl2 != 0)
			s->sflags |= S_SL2;
		if (s->u.note.notes[i].ti1 != 0)
			s->sflags |= S_TI1;
	}

	switch (cfmt.shiftunison) {
	case 0:
		break;
	case 1:
		s->sflags |= S_SHIFTUNISON_1;
		break;
	case 2:
		s->sflags |= S_SHIFTUNISON_2;
		break;
	default:
		s->sflags |= S_SHIFTUNISON_1 | S_SHIFTUNISON_2;
		break;
	}

	/* build the guitar chords */
	if (s->text)
		gch_build(s);
}