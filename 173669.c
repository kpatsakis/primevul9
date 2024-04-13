static void do_clip(void)
{
	struct SYMBOL *s, *s2;
	struct SYSTEM *sy;
	struct VOICE_S *p_voice;
	int voice;

	/* remove the beginning of the tune */
	s = tsfirst;
	if (clip_start.bar > 0
	 || clip_start.time > 0) {
		s = go_global_time(s, &clip_start);
		if (!s) {
			tsfirst = NULL;
			return;
		}

		/* update the start of voices */
		sy = cursys;
		for (s2 = tsfirst; s2 != s; s2 = s2->ts_next) {
			if (s->sflags & S_NEW_SY)
				sy = sy->next;
			switch (s2->type) {
			case CLEF:
				voice_tb[s2->voice].s_clef = s2;
				break;
			case KEYSIG:
				memcpy(&voice_tb[s2->voice].key, &s2->u.key,
					sizeof voice_tb[0].key);
				break;
			case TIMESIG:
				memcpy(&voice_tb[s2->voice].meter, &s2->u.meter,
					sizeof voice_tb[0].meter);
				break;
			}
		}
		cursys = sy;
		for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
			voice = p_voice - voice_tb;
			for (s2 = s; s2; s2 = s2->ts_next) {
				if (s2->voice == voice) {
					s2->prev = NULL;
					break;
				}
			}
			p_voice->sym = s2;
		}
		tsfirst = s;
		s->ts_prev = NULL;
	}

	/* remove the end of the tune */
	s = go_global_time(s, &clip_end);
	if (!s)
		return;

	/* keep the current sequence */
	do {
		s = s->ts_next;
		if (!s)
			return;
	} while (!(s->sflags & S_SEQST));

	/* cut the voices */
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		voice = p_voice - voice_tb;
		for (s2 = s->ts_prev; s2; s2 = s2->ts_prev) {
			if (s2->voice == voice) {
				s2->next = NULL;
				break;
			}
		}
		if (!s2)
			p_voice->sym = NULL;
	}
	s->ts_prev->ts_next = NULL;
}