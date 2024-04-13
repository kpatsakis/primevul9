struct SYMBOL *sym_add(struct VOICE_S *p_voice, int type)
{
	struct SYMBOL *s;
	struct VOICE_S *p_voice2;

	s = (struct SYMBOL *) getarena(sizeof *s);
	memset(s, 0, sizeof *s);
	p_voice2 = curvoice;
	curvoice = p_voice;
	sym_link(s, type);
	curvoice = p_voice2;
	if (p_voice->second)
		s->sflags |= S_SECOND;
	if (p_voice->floating)
		s->sflags |= S_FLOATING;
	if (s->prev) {
		s->fn = s->prev->fn;
		s->linenum = s->prev->linenum;
		s->colnum = s->prev->colnum;
	}
	return s;
}