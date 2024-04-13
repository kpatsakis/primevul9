static void voice_compress(void)
{
	struct VOICE_S *p_voice;
	struct SYMBOL *s, *s2, *s3, *ns;

	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
//8.7.0 - for fmt at end of music line
//		if (p_voice->ignore)
//			continue;
		p_voice->ignore = 0;
		for (s = p_voice->sym; s; s = s->next) {
			if (s->time >= staves_found)
				break;
		}
		ns = NULL;
		for ( ; s; s = s->next) {
			switch (s->type) {
#if 0 // test
			case KEYSIG:	/* remove the empty key signatures */
				if (s->u.key.empty) {
					if (s->prev)
						s->prev->next = s->next;
					else
						p_voice->sym = s->next;
					if (s->next)
						s->next->prev = s->prev;
					continue;
				}
				break;
#endif
			case FMTCHG:
				s2 = s->extra;
				if (s2) {	/* dummy format */
					if (!ns)
						ns = s2;
					if (s->prev) {
						s->prev->next = s2;
						s2->prev = s->prev;
					}
					if (!s->next) {
						ns = NULL;
						break;
					}
					while (s2->next)
						s2 = s2->next;
					s->next->prev = s2;
					s2->next = s->next;
				}
				/* fall thru */
			case TEMPO:
			case PART:
			case TUPLET:
				if (!ns)
					ns = s;
				continue;
			case MREST:		/* don't shift P: and Q: */
				if (!ns)
					continue;
				s2 = (struct SYMBOL *) getarena(sizeof *s);
				memset(s2, 0, sizeof *s2);
				s2->type = SPACE;
				s2->u.note.notes[1].len = -1;
				s2->flags = ABC_F_INVIS;
				s2->voice = s->voice;
				s2->staff = s->staff;
				s2->time = s->time;
				s2->sflags = s->sflags;
				s2->next = s;
				s2->prev = s->prev;
				s2->prev->next = s2;
				s->prev = s2;
				s = s2;
				break;
			}
			if (s->flags & ABC_F_GRACE) {
				if (!ns)
					ns = s;
				while (!(s->flags & ABC_F_GR_END))
					s = s->next;
				s2 = (struct SYMBOL *) getarena(sizeof *s);
				memcpy(s2, s, sizeof *s2);
				s2->abc_type = 0;
				s2->type = GRACE;
				s2->dur = 0;
				s2->next = s->next;
				if (s2->next) {
					s2->next->prev = s2;
					if (cfmt.graceword) {
						for (s3 = s2->next; s3; s3 = s3->next) {
							switch (s3->type) {
							case SPACE:
								continue;
							case NOTEREST:
								s2->ly = s3->ly;
								s3->ly = NULL;
							default:
								break;
							}
							break;
						}
					}
				} else {
					p_voice->last_sym = s2;
				}
				s2->prev = s;
				s->next = s2;
				s = s2;

				// with w_tb[BAR] = 2,
				// the grace notes go after the bar
				// if before a bar, change the grace time
				if (s->next && s->next->type == BAR)
					s->time--;
			}
			if (!ns)
				continue;
			s->extra = ns;
			s->prev->next = NULL;
			s->prev = ns->prev;
			if (s->prev)
				s->prev->next = s;
			else
				p_voice->sym = s;
			ns->prev = NULL;
			ns = NULL;
		}

		/* when symbols with no space at end of tune,
		 * add a dummy format */
		if (ns) {
			s = sym_add(p_voice, FMTCHG);
			s->aux = -1;		/* nothing */
			s->extra = ns;
			s->prev->next = NULL;	/* unlink */
			s->prev = ns->prev;
			if (s->prev)
				s->prev->next = s;
			else
				p_voice->sym = s;
			ns->prev = NULL;
		}
	}
}