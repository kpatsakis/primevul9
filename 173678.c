static void set_tuplet(struct SYMBOL *t)
{
	struct SYMBOL *s, *s1;
	int l, r, lplet, grace;

	r = t->u.tuplet.r_plet;
	grace = t->flags & ABC_F_GRACE;

	l = 0;
	for (s = t->abc_next; s; s = s->abc_next) {
		if (s->abc_type == ABC_T_TUPLET) {
			struct SYMBOL *s2;
			int l2, r2;

			r2 = s->u.tuplet.r_plet;
			l2 = 0;
			for (s2 = s->abc_next; s2; s2 = s2->abc_next) {
				switch (s2->abc_type) {
				case ABC_T_NOTE:
				case ABC_T_REST:
					break;
				case ABC_T_EOLN:
					if (s2->u.eoln.type != 1) {
						error(1, t,
							"End of line found inside a nested tuplet");
						return;
					}
					continue;
				default:
					continue;
				}
				if (s2->u.note.notes[0].len == 0)
					continue;
				if (grace ^ (s2->flags & ABC_F_GRACE))
					continue;
				s1 = s2;
				l2 += s1->dur;
				if (--r2 <= 0)
					break;
			}
			l2 = l2 * s->u.tuplet.q_plet / s->u.tuplet.p_plet;
			s->aux = l2;
			l += l2;
			r -= s->u.tuplet.r_plet;
			if (r == 0)
				break;
			if (r < 0) {
				error(1, t, "Bad nested tuplet");
				break;
			}
			s = s2;
			continue;
		}
		switch (s->abc_type) {
		case ABC_T_NOTE:
		case ABC_T_REST:
			break;
		case ABC_T_EOLN:
			if (s->u.eoln.type != 1) {
				error(1, t, "End of line found inside a tuplet");
				return;
			}
			continue;
		default:
			continue;
		}
		if (s->u.note.notes[0].len == 0)	/* space ('y') */
			continue;
		if (grace ^ (s->flags & ABC_F_GRACE))
			continue;
		s1 = s;
		l += s->dur;
		if (--r <= 0)
			break;
	}
	if (!s) {
		error(1, t, "End of tune found inside a tuplet");
		return;
	}
	if (t->aux != 0)		/* if nested tuplet */
		lplet = t->aux;
	else
		lplet = (l * t->u.tuplet.q_plet) / t->u.tuplet.p_plet;
	r = t->u.tuplet.r_plet;
	for (s = t->abc_next; s; s = s->abc_next) {
		int olddur;

		if (s->abc_type == ABC_T_TUPLET) {
			int r2;

			r2 = s->u.tuplet.r_plet;
			s1 = s;
			olddur = s->aux;
			s1->aux = (olddur * lplet) / l;
			l -= olddur;
			lplet -= s1->aux;
			r -= r2;
			for (;;) {
				s = s->abc_next;
				if (s->abc_type != ABC_T_NOTE
				 && s->abc_type != ABC_T_REST)
					continue;
				if (s->u.note.notes[0].len == 0)
					continue;
				if (grace ^ (s->flags & ABC_F_GRACE))
					continue;
				if (--r2 <= 0)
					break;
			}
			if (r <= 0)
				goto done;
			continue;
		}
		if (s->abc_type != ABC_T_NOTE && s->abc_type != ABC_T_REST)
			continue;
		if (s->u.note.notes[0].len == 0)
			continue;
		s->sflags |= S_IN_TUPLET;
		if (grace ^ (s->flags & ABC_F_GRACE))
			continue;
		s1 = s;
		olddur = s->dur;
		s1->dur = (olddur * lplet) / l;
		if (--r <= 0)
			break;
		l -= olddur;
		lplet -= s1->dur;
	}
done:
	if (grace) {
		error(1, t, "Tuplets in grace note sequence not yet treated");
	} else {
		sym_link(t, TUPLET);
		t->aux = cfmt.tuplets;
	}
}