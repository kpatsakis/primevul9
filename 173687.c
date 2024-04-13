static void get_key(struct SYMBOL *s)
{
	struct VOICE_S *p_voice;
	struct SYMBOL *s2;
	struct key_s okey;			/* original key */
	int i;
// int delta;

	if (s->u.key.octave != NO_OCTAVE)
		curvoice->octave = s->u.key.octave;
	if (s->u.key.cue > 0)
		curvoice->scale = 0.7;
	else if (s->u.key.cue < 0)
		curvoice->scale = 1;
	if (s->u.key.stafflines)
		curvoice->stafflines = s->u.key.stafflines;
	if (s->u.key.staffscale != 0)
		curvoice->staffscale = s->u.key.staffscale;

	if (s->u.key.empty == 1)		/* clef only */
		return;

	if (s->u.key.sf != 0
	 && !s->u.key.exp
	 && s->u.key.nacc != 0)
		set_k_acc(s);

	memcpy(&okey, &s->u.key, sizeof okey);
	if (s->state == ABC_S_HEAD) {		/* if first K: (start of tune) */
		for (i = MAXVOICE, p_voice = voice_tb;
		     --i >= 0;
		     p_voice++)
			p_voice->transpose = cfmt.transpose;
//		curvoice->transpose = cfmt.transpose;
	}
	if (curvoice->transpose != 0) {
		key_transpose(&s->u.key);

#if 0
		/* transpose explicit accidentals */
//fixme: not correct - transpose adds or removes accidentals...
		if (s->u.key.nacc > 0) {
			struct VOICE_S voice, *voice_sav;
			struct SYMBOL note;

			memset(&voice, 0, sizeof voice);
			voice.transpose = curvoice->transpose;
			memcpy(&voice.ckey, &s->u.key, sizeof voice.ckey);
			voice.ckey.empty = 2;
			voice.ckey.nacc = 0;
			memset(&note, 0, sizeof note);
--fixme
			memcpy(note.u.note.pits, voice.ckey.pits,
					sizeof note.u.note.pits);
			memcpy(note.u.note.accs, voice.ckey.accs,
					sizeof note.u.note.accs);
			note.nhd = s->u.key.nacc;
			voice_sav = curvoice;
			curvoice = &voice;
			note_transpose(&note);
			memcpy(s->u.key.pits, note.u.note.pits,
					sizeof s->u.key.pits);
			memcpy(s->u.key.accs, note.u.note.accs,
					sizeof s->u.key.accs);
			curvoice = voice_sav;
		}
#endif
	}

	// calculate the tonic delta
//	s->u.key.key_delta = (cgd2cde[(s->u.key.sf + 7) % 7] + 14 + s->u.key.mode) % 7;
	s->u.key.key_delta = (cgd2cde[(s->u.key.sf + 7) % 7] + 14) % 7;

	if (s->state == ABC_S_HEAD) {	/* start of tune */
		for (i = MAXVOICE, p_voice = voice_tb;
		     --i >= 0;
		     p_voice++) {
			memcpy(&p_voice->key, &s->u.key,
						sizeof p_voice->key);
			memcpy(&p_voice->ckey, &s->u.key,
						sizeof p_voice->ckey);
			memcpy(&p_voice->okey, &okey,
						sizeof p_voice->okey);
			if (p_voice->key.empty)
				p_voice->key.sf = 0;
			if (s->u.key.octave != NO_OCTAVE)
				p_voice->octave = s->u.key.octave;
			if (s->u.key.stafflines)
				p_voice->stafflines = s->u.key.stafflines;
			if (s->u.key.staffscale != 0)
				p_voice->staffscale = s->u.key.staffscale;
//fixme: update parsys->voice[voice].stafflines = stafflines; ?
		}
		return;
	}

	/* ABC_S_TUNE (K: cannot be ABC_S_GLOBAL) */
	if (is_tune_sig()) {

		/* define the starting key signature */
		memcpy(&curvoice->key, &s->u.key,
					sizeof curvoice->key);
		memcpy(&curvoice->ckey, &s->u.key,
					sizeof curvoice->ckey);
		memcpy(&curvoice->okey, &okey,
					sizeof curvoice->okey);
		switch (curvoice->key.instr) {
		case 0:
			if (!pipeformat) {
//				curvoice->transpose = cfmt.transpose;
				break;
			}
			//fall thru
		case K_HP:
		case K_Hp:
			if (curvoice->posit.std == 0)
				curvoice->posit.std = SL_BELOW;
			break;
		}
		if (curvoice->key.empty)
			curvoice->key.sf = 0;
		return;
	}

	/* key signature change */
	if ((!s->abc_next
	  || s->abc_next->abc_type != ABC_T_CLEF)	/* if not explicit clef */
	 && curvoice->ckey.sf == s->u.key.sf	/* and same key */
	 && curvoice->ckey.nacc == 0
	 && s->u.key.nacc == 0
	 && curvoice->ckey.empty == s->u.key.empty
	 && cfmt.keywarn)			/* (if not key warning,
						 *  keep all key signatures) */
		return;				/* ignore */

	if (!curvoice->ckey.empty)
		s->aux = curvoice->ckey.sf;	/* previous key signature */
	memcpy(&curvoice->ckey, &s->u.key,
				sizeof curvoice->ckey);
	memcpy(&curvoice->okey, &okey,
				sizeof curvoice->okey);
	if (s->u.key.empty)
		s->u.key.sf = 0;

	/* the key signature must appear before a time signature */
	s2 = curvoice->last_sym;
	if (s2 && s2->type == TIMESIG) {
		curvoice->last_sym = s2->prev;
		if (!curvoice->last_sym)
			curvoice->sym = NULL;
		sym_link(s, KEYSIG);
		s->next = s2;
		s2->prev = s;
		curvoice->last_sym = s2;
	} else {
		sym_link(s, KEYSIG);
	}
}