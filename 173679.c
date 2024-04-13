void sort_pitch(struct SYMBOL *s)
{
	int i, nx, k;
	struct note v_note;
	unsigned char new_order[MAXHD], inv_order[MAXHD];

	for (i = 0; i <= s->nhd; i++)
		new_order[i] = i;
	for (;;) {
		nx = 0;
		for (i = 1; i <= s->nhd; i++) {
			if (s->u.note.notes[i].pit >= s->u.note.notes[i - 1].pit)
				continue;
			memcpy(&v_note, &s->u.note.notes[i],
					sizeof v_note);
			memcpy(&s->u.note.notes[i], &s->u.note.notes[i - 1],
					sizeof v_note);
			memcpy(&s->u.note.notes[i - 1], &v_note,
					sizeof v_note);
			k = s->pits[i];
			s->pits[i] = s->pits[i - 1];
			s->pits[i - 1] = k;
			k = new_order[i];
			new_order[i] = new_order[i - 1];
			new_order[i - 1] = k;
			nx++;
		}
		if (nx == 0)
			break;
	}

	/* change the indexes of the note head decorations */
	if (s->nhd > 0) {
		for (i = 0; i <= s->nhd; i++)
			inv_order[new_order[i]] = i;
		for (i = 0; i <= s->u.note.dc.n; i++) {
			k = s->u.note.dc.tm[i].m;
			if (k >= 0)
				s->u.note.dc.tm[i].m = inv_order[k];
		}
	}
}