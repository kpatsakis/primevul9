static struct SYMBOL *go_global_time(struct SYMBOL *s,
				struct symsel_s *symsel)
{
	struct SYMBOL *s2;
	int bar_time;

	if (symsel->bar <= 1) {		/* special case: there is no measure 0/1 */
//	 && nbar == -1) {		/* see set_bar_num */
		if (symsel->bar == 0)
			goto chk_time;
		for (s2 = s; s2; s2 = s2->ts_next) {
			if (s2->type == BAR
			 && s2->time != 0)
				break;
		}
		if (s2->time < voice_tb[cursys->top_voice].meter.wmeasure)
			s = s2;
		goto chk_time;
	}
	for ( ; s; s = s->ts_next) {
		if (s->type == BAR
		 && s->aux >= symsel->bar)
			break;
	}
	if (!s)
		return NULL;
	if (symsel->seq != 0) {
		int seq;

		seq = symsel->seq;
		for (s = s->ts_next; s; s = s->ts_next) {
			if (s->type == BAR
			 && s->aux == symsel->bar) {
				if (--seq == 0)
					break;
			}
		}
		if (!s)
			return NULL;
	}

chk_time:
	if (symsel->time == 0)
		return s;
	bar_time = s->time + symsel->time;
	while (s->time < bar_time) {
		s = s->ts_next;
		if (!s)
			return s;
	}
	do {
		s = s->ts_prev;		/* go back to the previous sequence */
	} while (!(s->sflags & S_SEQST));
	return s;
}