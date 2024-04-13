static void get_staves(struct SYMBOL *s)
{
	struct SYMBOL *s2;
	struct VOICE_S *p_voice, *p_voice2;
	struct staff_s *p_staff, staves[MAXVOICE];
	int i, flags, voice, staff, range, dup_voice, maxtime;

	voice_compress();
	voice_dup();

	/* create a new staff system */
	curvoice = p_voice = first_voice;
	maxtime = p_voice->time;
	flags = p_voice->sym != NULL;
	for (p_voice = p_voice->next; p_voice; p_voice = p_voice->next) {
		if (p_voice->time > maxtime)
			maxtime = p_voice->time;
		if (p_voice->sym)
			flags = 1;
	}
	if (flags == 0			/* if first %%staves */
	 || (maxtime == 0 && staves_found < 0)) {
		for (voice = 0; voice < MAXVOICE; voice++)
			parsys->voice[voice].range = -1;
	} else {

		/*
		 * create a new staff system and
		 * link the staves in a voice which is seen from
		 * the previous system - see sort_all
		 */
//		p_voice = curvoice;
		if (parsys->voice[curvoice - voice_tb].range < 0) {
			for (voice = 0; voice < MAXVOICE; voice++) {
				if (parsys->voice[voice].range >= 0) {
					curvoice = &voice_tb[voice];
					break;
				}
			}
/*fixme: should check if voice < MAXVOICE*/
		}
		curvoice->time = maxtime;

		// put the staves before a measure bar (see draw_bar())
		s2 = curvoice->last_sym;
		if (s2 && s2->type == BAR && s2->time == maxtime) {
			curvoice->last_sym = s2->prev;
			if (!curvoice->last_sym)
				curvoice->sym = NULL;
			sym_link(s, STAVES);
			s->next = s2;
			s2->prev = s;
			curvoice->last_sym = s2;
		} else {
			sym_link(s, STAVES); // link the staves in the current voice
		}
		s->state = ABC_S_HEAD; /* (output PS sequences immediately) */
		parsys->nstaff = nstaff;
		system_new();
	}
	staves_found = maxtime;

	memset(staves, 0, sizeof staves);
	parse_staves(s, staves);

	/* initialize the voices */
	for (voice = 0, p_voice = voice_tb;
	     voice < MAXVOICE;
	     voice++, p_voice++) {
		p_voice->second = 0;
		p_voice->floating = 0;
		p_voice->ignore = 0;
		p_voice->time = maxtime;
	}

	/* create the 'clone' voices */
	dup_voice = MAXVOICE;
	range = 0;
	p_staff = staves;
	parsys->top_voice = p_staff->voice;
	for (i = 0;
	     i < MAXVOICE && p_staff->voice >= 0;
	     i++, p_staff++) {
		voice = p_staff->voice;
		p_voice = &voice_tb[voice];
		if (parsys->voice[voice].range >= 0) {
			if (parsys->voice[dup_voice - 1].range >= 0) {
				error(1, s, "Too many voices for cloning");
				continue;
			}
			voice = --dup_voice;	/* duplicate the voice */
			p_voice2 = &voice_tb[voice];
			memcpy(p_voice2, p_voice, sizeof *p_voice2);
			p_voice2->next = NULL;
			p_voice2->sym = p_voice2->last_sym = NULL;
			p_voice2->tblts[0] = p_voice2->tblts[1] = NULL;
			p_voice2->clone = -1;
			while (p_voice->clone > 0)
				p_voice = &voice_tb[p_voice->clone];
			p_voice->clone = voice;
			p_voice = p_voice2;
			p_staff->voice = voice;
		}
		parsys->voice[voice].range = range++;
		voice_link(p_voice);
	}

	/* change the behavior from %%staves to %%score */
	if (s->text[3] == 't') {		/* if %%staves */
		for (i = 0, p_staff = staves;
		     i < MAXVOICE - 2 && p_staff->voice >= 0;
		     i++, p_staff++) {
			flags = p_staff->flags;
			if (!(flags & (OPEN_BRACE | OPEN_BRACE2)))
				continue;
			if ((flags & (OPEN_BRACE | CLOSE_BRACE))
					== (OPEN_BRACE | CLOSE_BRACE)
			 || (flags & (OPEN_BRACE2 | CLOSE_BRACE2))
					== (OPEN_BRACE2 | CLOSE_BRACE2))
				continue;
			if (p_staff[1].flags != 0)
				continue;
			if ((flags & OPEN_PARENTH)
			 || (p_staff[2].flags & OPEN_PARENTH))
				continue;

			/* {a b c} --> {a *b c} */
			if (p_staff[2].flags & (CLOSE_BRACE | CLOSE_BRACE2)) {
				p_staff[1].flags |= FL_VOICE;

			/* {a b c d} --> {(a b) (c d)} */
			} else if (p_staff[2].flags == 0
				 && (p_staff[3].flags & (CLOSE_BRACE | CLOSE_BRACE2))) {
				p_staff->flags |= OPEN_PARENTH;
				p_staff[1].flags |= CLOSE_PARENTH;
				p_staff[2].flags |= OPEN_PARENTH;
				p_staff[3].flags |= CLOSE_PARENTH;
			}
		}
	}

	/* set the staff system */
	staff = -1;
	for (i = 0, p_staff = staves;
	     i < MAXVOICE && p_staff->voice >= 0;
	     i++, p_staff++) {
		flags = p_staff->flags;
		if ((flags & (OPEN_PARENTH | CLOSE_PARENTH))
				== (OPEN_PARENTH | CLOSE_PARENTH)) {
			flags &= ~(OPEN_PARENTH | CLOSE_PARENTH);
			p_staff->flags = flags;
		}
		voice = p_staff->voice;
		p_voice = &voice_tb[voice];
		if (flags & FL_VOICE) {
			p_voice->floating = 1;
			p_voice->second = 1;
		} else {
#if MAXSTAFF < MAXVOICE
			if (staff >= MAXSTAFF - 1) {
				error(1, s, "Too many staves");
			} else
#endif
				staff++;
			parsys->staff[staff].flags = 0;
		}
		p_voice->staff = p_voice->cstaff
				= parsys->voice[voice].staff = staff;
		parsys->staff[staff].flags |= flags;
		if (flags & OPEN_PARENTH) {
			p_voice2 = p_voice;
			while (i < MAXVOICE) {
				i++;
				p_staff++;
				voice = p_staff->voice;
				p_voice = &voice_tb[voice];
				if (p_staff->flags & MASTER_VOICE) {
					p_voice2->second = 1;
					p_voice2 = p_voice;
				} else {
					p_voice->second = 1;
				}
				p_voice->staff = p_voice->cstaff
						= parsys->voice[voice].staff
						= staff;
				if (p_staff->flags & CLOSE_PARENTH)
					break;
			}
			parsys->staff[staff].flags |= p_staff->flags;
		}
	}
	if (staff < 0)
		staff = 0;
	parsys->nstaff = nstaff = staff;

	/* change the behaviour of '|' in %%score */
	if (s->text[3] == 'c') {		/* if %%score */
		for (staff = 0; staff < nstaff; staff++)
			parsys->staff[staff].flags ^= STOP_BAR;
	}

	for (voice = 0; voice < MAXVOICE; voice++) {
		p_voice = &voice_tb[voice];
		parsys->voice[voice].second = p_voice->second;
		staff = p_voice->staff;
		if (staff > 0)
			p_voice->norepbra
				= !(parsys->staff[staff - 1].flags & STOP_BAR);
		if (p_voice->floating && staff == nstaff)
			p_voice->floating = 0;
	}
	curvoice = &voice_tb[parsys->top_voice];
}