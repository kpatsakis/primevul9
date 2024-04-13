static void ps_def(struct SYMBOL *s,
			char *p,
			char use)	/* cf user_ps_add() */
{
	if (!svg && epsf <= 1) {		/* if PS output */
		if (secure
//		 || use == 'g'		// SVG
		 || use == 's')		// PS for SVG
			return;
	} else {				/* if SVG output */
		if (use == 'p'		// PS for PS
		 || (use == 'g'		// SVG
		  && file_initialized > 0))
			return;
	}
	if (s->abc_prev)
		s->state = s->abc_prev->state;
	if (s->state == ABC_S_TUNE) {
		if (use == 'g')		// SVG
			return;
		sym_link(s, FMTCHG);
		s->aux = PSSEQ;
		s->text = p;
//		s->flags |= ABC_F_INVIS;
		return;
	}
	if (use == 'g') {			// SVG
		svg_ps(p);
		if (!svg && epsf <= 1)
			return;
	}
	if (file_initialized > 0 || mbf != outbuf)
		a2b("%s\n", p);
	else
		user_ps_add(p, use);
}