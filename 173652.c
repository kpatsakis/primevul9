static void adjust_dur(struct SYMBOL *s)
{
	struct SYMBOL *s2;
	int time, auto_time;

	/* search the start of the measure */
	s2 = curvoice->last_sym;
	if (!s2)
		return;

	/* the bar time is correct if there is multi-rests */
	if (s2->type == MREST
	 || s2->type == BAR)		/* in second voice */
		return;
	while (s2->type != BAR && s2->prev)
		s2 = s2->prev;
	time = s2->time;
	auto_time = curvoice->time - time;

	/* remove the invisible rest at start of tune */
	if (time == 0) {
		while (s2 && s2->dur == 0)
			s2 = s2->next;
		if (s2 && s2->abc_type == ABC_T_REST
		 && (s2->flags & ABC_F_INVIS)) {
			time += s2->dur * curvoice->wmeasure / auto_time;
			if (s2->prev)
				s2->prev->next = s2->next;
			else
				curvoice->sym = s2->next;
			if (s2->next)
				s2->next->prev = s2->prev;
			s2 = s2->next;
		}
	}
	if (curvoice->wmeasure == auto_time)
		return;				/* already good duration */

	for (; s2; s2 = s2->next) {
		int i, head, dots, nflags;

		s2->time = time;
		if (s2->dur == 0
		 || (s2->flags & ABC_F_GRACE))
			continue;
		s2->dur = s2->dur * curvoice->wmeasure / auto_time;
		time += s2->dur;
		if (s2->type != NOTEREST)
			continue;
		for (i = 0; i <= s2->nhd; i++)
			s2->u.note.notes[i].len = s2->u.note.notes[i].len
					 * curvoice->wmeasure / auto_time;
		identify_note(s2, s2->u.note.notes[0].len,
				&head, &dots, &nflags);
		s2->head = head;
		s2->dots = dots;
		s2->nflags = nflags;
		if (s2->nflags <= -2)
			s2->flags |= ABC_F_STEMLESS;
		else
			s2->flags &= ~ABC_F_STEMLESS;
	}
	curvoice->time = s->time = time;
}