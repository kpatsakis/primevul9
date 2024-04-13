static void sym_link(struct SYMBOL *s, int type)
{
	struct VOICE_S *p_voice = curvoice;

	if (!p_voice->ignore) {
		s->prev = p_voice->last_sym;
		if (s->prev)
			p_voice->last_sym->next = s;
		else
			p_voice->sym = s;
		p_voice->last_sym = s;
//fixme:test bug
//	} else {
//		if (p_voice->sym)
//			p_voice->last_sym = p_voice->sym = s;
	}

	s->type = type;
	s->voice = p_voice - voice_tb;
	s->staff = p_voice->cstaff;
	s->time = p_voice->time;
	s->posit = p_voice->posit;
}