static void voice_dup(void)
{
	struct VOICE_S *p_voice, *p_voice2;
	struct SYMBOL *s, *s2, *g, *g2;
	int voice;

	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		if ((voice = p_voice->clone) < 0)
			continue;
		p_voice->clone = -1;
		p_voice2 = &voice_tb[voice];
		for (s = p_voice->sym; s; s = s->next) {
//fixme: there may be other symbols before the %%staves at this same time
			if (s->time >= staves_found)
				break;
		}
		for ( ; s; s = s->next) {
			if (s->type == STAVES)
				continue;
			s2 = (struct SYMBOL *) getarena(sizeof *s2);
			memcpy(s2, s, sizeof *s2);
			s2->prev = p_voice2->last_sym;
			s2->next = NULL;
			if (p_voice2->sym)
				p_voice2->last_sym->next = s2;
			else
				p_voice2->sym = s2;
			p_voice2->last_sym = s2;
			s2->voice = voice;
			s2->staff = p_voice2->staff;
			if (p_voice2->second)
				s2->sflags |= S_SECOND;
			else
				s2->sflags &= ~S_SECOND;
			if (p_voice2->floating)
				s2->sflags |= S_FLOATING;
			else
				s2->sflags &= ~S_FLOATING;
			s2->ly = NULL;
			g = s2->extra;
			if (!g)
				continue;
			g2 = (struct SYMBOL *) getarena(sizeof *g2);
			memcpy(g2, g, sizeof *g2);
			s2->extra = g2;
			s2 = g2;
			s2->voice = voice;
			s2->staff = p_voice2->staff;
			for (g = g->next; g; g = g->next) {
				g2 = (struct SYMBOL *) getarena(sizeof *g2);
				memcpy(g2, g, sizeof *g2);
				s2->next = g2;
				g2->prev = s2;
				s2 = g2;
				s2->voice = voice;
				s2->staff = p_voice2->staff;
			}
		}
	}
}