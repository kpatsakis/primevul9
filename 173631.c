static int acc_same_pitch(int pitch)
{
	struct SYMBOL *s = curvoice->last_sym->prev;
	int i, time;

	// the overlaid voices may have no measure bars
//	if (curvoice->id[0] == '&')
//		s = voice_tb[curvoice->mvoice].last_sym;

	if (!s)
		return -1;

	time = s->time;

	for (; s; s = s->prev) {
		switch (s->abc_type) {
		case ABC_T_BAR:
			if (s->time < time)
				return -1;	/* no same pitch */
			for (;;) {
				s = s->prev;
				if (!s)
					return -1;
				if (s->abc_type == ABC_T_NOTE) {
					if (s->time + s->dur == time)
						break;
					return -1;
				}
				if (s->time < time)
					return -1;
			}
			for (i = 0; i <= s->nhd; i++) {
				if (s->u.note.notes[i].pit == pitch
				 && s->u.note.notes[i].ti1)
					return s->u.note.notes[i].acc;
			}
			return -1;
		case ABC_T_NOTE:
			for (i = 0; i <= s->nhd; i++) {
				if (s->u.note.notes[i].pit == pitch)
					return s->u.note.notes[i].acc;
			}
			break;
		}
	}
	return -1;
}