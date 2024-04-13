static void set_bar_num(void)
{
	struct SYMBOL *s, *s2, *s3;
	int bar_time, wmeasure, tim;
	int bar_num, bar_rep;

	wmeasure = voice_tb[cursys->top_voice].meter.wmeasure;
	bar_rep = nbar;

	/* don't count a bar at start of line */
	for (s = tsfirst; ; s = s->ts_next) {
		if (!s)
			return;
		switch (s->type) {
		case TIMESIG:
		case CLEF:
		case KEYSIG:
		case FMTCHG:
		case STBRK:
			continue;
		case BAR:
			if (s->aux) {
				nbar = s->aux;		/* (%%setbarnb) */
				break;
			}
			if (s->u.bar.repeat_bar
			 && s->text
			 && !cfmt.contbarnb) {
				if (s->text[0] == '1') {
					bar_rep = nbar;
				} else {
					nbar = bar_rep; /* restart bar numbering */
					s->aux = nbar;
				}
			}
			break;
		}
		break;
	}

	/* set the measure number on the top bars
	 * and move the clefs before the measure bars */
	bar_time = s->time + wmeasure;	/* for incomplete measure at start of tune */
	bar_num = nbar;
	for ( ; s; s = s->ts_next) {
		switch (s->type) {
		case CLEF:
			if (s->sflags & S_NEW_SY)
				break;
			for (s2 = s->ts_prev; s2; s2 = s2->ts_prev) {
				if (s2->sflags & S_NEW_SY) {
					s2 = NULL;
					break;
				}
				switch (s2->type) {
				case BAR:
					if (s2->sflags & S_SEQST)
						break;
					continue;
				case MREST:
				case NOTEREST:
				case SPACE:
				case STBRK:
				case TUPLET:
					s2 = NULL;
					break;
				default:
					continue;
				}
				break;
			}
			if (!s2)
				break;

			/* move the clef */
			s->next->prev = s->prev;
			s->prev->next = s->next;
			s->ts_next->ts_prev = s->ts_prev;
			s->ts_prev->ts_next = s->ts_next;
			s->next = s2;
			s->prev = s2->prev;
			s->prev->next = s;
			s2->prev = s;
			s->ts_next = s2;
			s->ts_prev = s2->ts_prev;
			s->ts_prev->ts_next = s;
			s2->ts_prev = s;
//			if (s->sflags & S_NEW_SY) {
//				s->sflags &= ~S_NEW_SY;
//				s->ts_next->sflags |= S_NEW_SY;
//			}
			s3 = s->extra;
			if (s3) {
				if (s->ts_next->extra) {
					while (s3->next)
						s3 = s3->next;
					s3->next = s->ts_next->extra;
					s->ts_next->extra = s->extra;
				} else {
					s->ts_next->extra = s3;
				}
				s->extra = NULL;
			}
			s = s2;
			break;
		case TIMESIG:
			wmeasure = s->u.meter.wmeasure;
			if (s->time < bar_time)
				bar_time = s->time + wmeasure;
			break;
		case MREST:
			bar_num += s->u.bar.len - 1;
			while (s->ts_next
			    && s->ts_next->type != BAR)
				s = s->ts_next;
			break;
		case BAR:
//			if (s->flags & ABC_F_INVIS)
//				break;
			if (s->aux) {
				bar_num = s->aux;		/* (%%setbarnb) */
//				if (s->time < bar_time) {
//					s->aux = 0;
					break;
//				}
			} else {
				if (s->time < bar_time)	/* incomplete measure */
					break;
				bar_num++;
			}

			/* check if any repeat bar at this time */
			tim = s->time;
			s2 = s;
			do {
				if (s2->type == BAR
				 && s2->u.bar.repeat_bar
				 && s2->text
				 && !cfmt.contbarnb) {
					if (s2->text[0] == '1')
						bar_rep = bar_num;
					else		/* restart bar numbering */
						bar_num = bar_rep;
					break;
				}
				s2 = s2->next;
			} while (s2 && s2->time == tim);
			s->aux = bar_num;
			bar_time = s->time + wmeasure;
			break;
		}
	}

	/* do the %%clip stuff */
	if (clip_start.bar >= 0) {
		if (bar_num <= clip_start.bar
		 || nbar > clip_end.bar) {
			tsfirst = NULL;
			return;
		}
		do_clip();
	}

	/* do the %%break stuff */
	{
		struct brk_s *brk;
		int nbar_min;

//		if (nbar == 1)
//			nbar = -1;	/* see go_global_time */
		nbar_min = nbar;
		if (nbar_min == 1)
			nbar_min = -1;
		for (brk = brks; brk; brk = brk->next) {
			if (brk->symsel.bar <= nbar_min
			 || brk->symsel.bar > bar_num)
				continue;
			s = go_global_time(tsfirst, &brk->symsel);
			if (s)
				s->sflags |= S_EOLN;
		}
	}
	if (cfmt.measurenb < 0)		/* if no display of measure bar */
		nbar = bar_num;		/* update in case of more music to come */
}