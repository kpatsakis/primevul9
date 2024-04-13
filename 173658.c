void do_tune(void)
{
	struct VOICE_S *p_voice;
	struct SYMBOL *s, *s1, *s2;
	int i;

	/* initialize */
	lvlarena(0);
	nstaff = 0;
	staves_found = -1;
	for (i = 0; i < MAXVOICE; i++) {
		p_voice = &voice_tb[i];
		s1 = (struct SYMBOL *) getarena(sizeof *s1);
		memset(s1, 0, sizeof *s1);
		s1->type = CLEF;
		s1->voice = i;
		if (cfmt.autoclef) {
			s1->u.clef.type = AUTOCLEF;
			s1->sflags = S_CLEF_AUTO;
		} else {
			s1->u.clef.type = TREBLE;
		}
		s1->u.clef.line = 2;		/* treble clef on 2nd line */
		p_voice->s_clef = s1;
		p_voice->meter.wmeasure = 1;	// M:none
		p_voice->wmeasure = 1;
		p_voice->scale = 1;
		p_voice->clone = -1;
		p_voice->over = -1;
		p_voice->posit = cfmt.posit;
		p_voice->stafflines = NULL;
//		p_voice->staffscale = 0;
	}
	curvoice = first_voice = voice_tb;
	reset_deco();
	abc2win = 0;
	clip_start.bar = -1;
	clip_end.bar = (short unsigned) ~0 >> 1;

	parsys = NULL;
	system_new();			/* create the 1st staff system */
	parsys->top_voice = parsys->voice[0].range = 0;	/* implicit voice */

	if (!epsf) {
//fixme: 8.6.2
#if 1
// fixme: should already be 0
		use_buffer = 0;
#else
		if (cfmt.oneperpage) {
			use_buffer = 0;
			close_page();
		} else {
			if (in_page)		// ??
				use_buffer = cfmt.splittune != 1;
		}
#endif
	} else {
		use_buffer = 1;
		marg_init();
	}

	/* set the duration of all notes/rests
	 *	(this is needed for tuplets and the feathered beams) */
	for (s = parse.first_sym; s; s = s->abc_next) {
		switch (s->abc_type) {
		case ABC_T_EOLN:
			if (s->u.eoln.type == 2)
				abc2win = 1;
			break;
		case ABC_T_NOTE:
		case ABC_T_REST:
			s1 = s;
			s1->dur = s1->u.note.notes[0].len;
			break;
		}
	}

	if (voice_tb[0].id[0] == '\0') {	/* single voice */
		voice_tb[0].id[0] = '1';	/* implicit V:1 */
		voice_tb[0].id[1] = '\0';
	}

	/* scan the tune */
	for (s = parse.first_sym; s; s = s->abc_next) {
		s1 = s;
		if (s->flags & ABC_F_LYRIC_START)
			curvoice->lyric_start = curvoice->last_sym;
		switch (s->abc_type) {
		case ABC_T_INFO:
			s = get_info(s);
			break;
		case ABC_T_PSCOM:
			s = process_pscomment(s);
			break;
		case ABC_T_NOTE:
		case ABC_T_REST:
			if (curvoice->space
			 && !(s->flags & ABC_F_GRACE)) {
				curvoice->space = 0;
				s1->flags |= ABC_F_SPACE;
			}
			get_note(s1);
			break;
		case ABC_T_BAR:
			if (over_bar)
				get_over(s1);
			get_bar(s1);
			break;
		case ABC_T_CLEF:
			get_clef(s1);
			break;
		case ABC_T_EOLN:
			if (cfmt.breakoneoln
			 || (s->flags & ABC_F_SPACE))
				curvoice->space = 1;
			if (cfmt.continueall || cfmt.barsperstaff
			 || s->u.eoln.type == 1)	/* if '\' */
				continue;
			if (s->u.eoln.type == 0		/* if normal eoln */
			 && abc2win
			 && parse.abc_vers != (2 << 16))
				continue;
			if (parsys->voice[curvoice - voice_tb].range == 0
			 && curvoice->last_sym)
				curvoice->last_sym->sflags |= S_EOLN;
			if (!cfmt.alignbars)
				continue;		/* normal */

			/* align bars */
			while (s->abc_next) {		/* treat the lyrics */
				if (s->abc_next->abc_type != ABC_T_INFO)
					break;
				switch (s->abc_next->text[0]) {
				case 'w':
					s = get_info(s->abc_next);
					s1 = s;
					continue;
				case 'd':
				case 's':
					s = s->abc_next;
					s1 = s;
					continue;
				}
				break;
			}
			i = (curvoice - voice_tb) + 1;
			if (i < cfmt.alignbars) {
				curvoice = &voice_tb[i];
				continue;
			}
			generate();
			buffer_eob(0);
			curvoice = &voice_tb[0];
			continue;
		case ABC_T_MREST: {
			int dur;

			dur = curvoice->wmeasure * s->u.bar.len;
			if (curvoice->second) {
				curvoice->time += dur;
				break;
			}
			sym_link(s1, MREST);
			s->dur = dur;
			curvoice->time += dur;
			if (s1->text)
				gch_build(s1);	/* build the guitar chords */
			if (s1->u.bar.dc.n > 0)
				deco_cnv(&s1->u.bar.dc, s, NULL);
			break;
		    }
		case ABC_T_MREP: {
			int n;

			if (!s->abc_next || s->abc_next->abc_type != ABC_T_BAR) {
				error(1, s1,
				      "Measure repeat not followed by a bar");
				break;
			}
			if (curvoice->ignore)
				break;
			n = s->u.bar.len;
			if (curvoice->second) {
				curvoice->time += curvoice->wmeasure * n;
				break;
			}
			s2 = sym_add(curvoice, NOTEREST);
			s2->abc_type = ABC_T_REST;
			s2->flags |= ABC_F_INVIS;
			s2->dur = curvoice->wmeasure;
			curvoice->time += s2->dur;
			if (n == 1) {
				s->abc_next->u.bar.len = n; /* <n> in the next bar */
				break;
			}
			while (--n > 0) {
				s2 = sym_add(curvoice, BAR);
				s2->u.bar.type = B_SINGLE;
				if (n == s->u.bar.len - 1)
					s2->u.bar.len = s->u.bar.len;
				s2 = sym_add(curvoice, NOTEREST);
				s2->abc_type = ABC_T_REST;
				s2->flags |= ABC_F_INVIS;
				s2->dur = curvoice->wmeasure;
				curvoice->time += s2->dur;
			}
			break;
		    }
		case ABC_T_V_OVER:
			get_over(s1);
			continue;
		case ABC_T_TUPLET:
			set_tuplet(s1);
			break;
		default:
			continue;
		}
		if (s1->type == 0)
			continue;
		if (curvoice->second)
			s1->sflags |= S_SECOND;
		if (curvoice->floating)
			s1->sflags |= S_FLOATING;
	}

	gen_ly(0);
	put_history();
	buffer_eob(1);
	if (epsf) {
		write_eps();
	} else {
		write_buffer();
//		if (!cfmt.oneperpage && in_page)
//			use_buffer = cfmt.splittune != 1;
	}

	if (info['X' - 'A']) {
		memcpy(&cfmt, &dfmt, sizeof cfmt); /* restore global values */
		memcpy(&info, &info_glob, sizeof info);
		memcpy(deco, deco_glob, sizeof deco);
		maps = maps_glob;
		info['X' - 'A'] = NULL;
	}

	/* free the parsing resources */
	{
		struct brk_s *brk, *brk2;

		brk = brks;
		while (brk) {
			brk2 = brk->next;
			free(brk);
			brk = brk2;
		}
		brks = brk;		/* (NULL) */
	}
}