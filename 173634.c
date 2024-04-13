static void mrest_expand(struct SYMBOL *s)
{
	struct VOICE_S *p_voice;
	struct SYMBOL *s2, *next;
	struct decos dc;
	int nb, dt;

	nb = s->u.bar.len;
	dt = s->dur / nb;

	/* change the multi-rest (type bar) to a single rest */
	memcpy(&dc, &s->u.bar.dc, sizeof dc);
	memset(&s->u.note, 0, sizeof s->u.note);
	s->type = NOTEREST;
	s->abc_type = ABC_T_REST;
//	s->nhd = 0;
	s->dur = s->u.note.notes[0].len = dt;
	s->head = H_FULL;
	s->nflags = -2;

	/* add the bar(s) and rest(s) */
	next = s->next;
	p_voice = &voice_tb[s->voice];
	p_voice->last_sym = s;
	p_voice->time = s->time + dt;
	p_voice->cstaff = s->staff;
	s2 = s;
	while (--nb > 0) {
		s2 = sym_add(p_voice, BAR);
		s2->abc_type = ABC_T_BAR;
		s2->u.bar.type = B_SINGLE;
		s2 = sym_add(p_voice, NOTEREST);
		s2->abc_type = ABC_T_REST;
		s2->flags = s->flags;
		s2->dur = s2->u.note.notes[0].len = dt;
		s2->head = H_FULL;
		s2->nflags = -2;
		p_voice->time += dt;
	}
	s2->next = next;
	if (next)
		next->prev = s2;

	/* copy the mrest decorations to the last rest */
	memcpy(&s2->u.note.dc, &dc, sizeof s2->u.note.dc);
}