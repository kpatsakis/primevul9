static void rcc_context(REgg *egg, int delta) {
	REggEmit *emit = egg->remit;
	char str[64];

	if (CTX > 31 || CTX < 0) {
		return;
	}

	if (delta > 0) {
		egg->lang.nestedi[CTX]++;
		R_FREE (egg->lang.nested_callname[CTX]);
		if (egg->lang.callname) {
			egg->lang.nested_callname[CTX] = strdup (egg->lang.callname);
		}
	}
	if (egg->lang.callname && CTX > 0) {	// && delta>0) {
		// set_nested (callname);
// eprintf (" - - - - - - -  set nested d=%d c=%d (%s)\n", delta, context-1, callname);
// shownested();
	}
	CTX += delta;
	egg->lang.lastctxdelta = delta;

	if (CTX == 0 && delta < 0) {
		if (egg->lang.mode != NAKED) {
			emit->frame_end (egg, egg->lang.stackframe + egg->lang.stackfixed, egg->lang.nbrackets);
		}
		if (egg->lang.mode == NORMAL) {	/* XXX : commenting this makes hello.r unhappy! TODO: find a cleaner alternative */
			egg->lang.stackframe = 0;
		}
		egg->lang.mode = NORMAL;
	} else {
		/* conditional block */
// eprintf ("Callname is (%s)\n", callname);
		const char *elm = skipspaces (egg->lang.elem);
		// const char *cn = callname;
		// seems cn is useless in nowdays content
// if (egg->lang.nested[context-1])
#if 0
		if (delta < 0 && context > 0) {
			eprintf ("close bracket foo!!!\n");
			shownested ();
			cn = strdup (egg->lang.nested[context - 1]);
			eprintf ("STATEMENT cn=(%s) idx=%d (%s)\n", cn, context - 1, egg->lang.nested[context - 1]);
			eprintf ("CNTXXXPUSH (%s)\n", egg->lang.ctxpush[context - 1]);
#if 0
			if (!strcmp (cn, "while")) {
				emit->while_end (egg, get_frame_label (context - 1));
				// char *var = get_frame_label (0);
				// emit->jmp (egg, var, 0);
				return;
			}
#endif
		}
#endif
// eprintf ("ELEM (%s)\n", elm);
// eprintf ("END BLOCK %d, (%s)\n", context, egg->lang.nested[context-1]);
// eprintf ("CN = (%s) %d (%s) delta=%d\n", cn, context, egg->lang.nested[context-1], delta);
		if (egg->lang.callname) {
			// if (callname) { // handle 'foo() {'
			/* TODO: this must be an array */
			char *b, *g, *e, *n;
			emit->comment (egg, "cond frame %s (%s)", egg->lang.callname, elm);
			/* TODO: simplify with a single for */
			if (egg->lang.conditionstr) {
				b = strchr (egg->lang.conditionstr, '<');	/* below */
				g = strchr (egg->lang.conditionstr, '>');	/* greater */
				e = strchr (egg->lang.conditionstr, '=');	/* equal */
				n = strchr (egg->lang.conditionstr, '!');	/* negate */
			} else {
				b = g = e = n = NULL;
			}
			if (!strcmp (egg->lang.callname, "while")) {
				char lab[128];
				sprintf (lab, "__begin_%d_%d_%d", egg->lang.nfunctions,
					CTX - 1, egg->lang.nestedi[CTX - 1] - 1);
				// the egg->lang.nestedi[CTX-1] has increased
				// so we should decrease it in label
				emit->get_while_end (egg, str, egg->lang.ctxpush[CTX - 1], lab);	// get_frame_label (2));
// get_frame_label (2));
// eprintf ("------ (%s)\n", egg->lang.ctxpush[context-1]);
				// free (egg->lang.endframe);
// XXX: egg->lang.endframe is deprecated, must use set_nested only
				if (delta > 0) {
					set_nested (egg, str);
				}
				rcc_set_callname (egg, "if");// append 'if' body
			}
			if (!strcmp (egg->lang.callname, "if")) {
				// emit->branch (egg, b, g, e, n, egg->lang.varsize, get_end_frame_label (egg));
				// HACK HACK :D
				// sprintf (str, "__end_%d_%d_%d", egg->lang.nfunctions,
				// CTX-1, egg->lang.nestedi[CTX-1]);
				// nestede[CTX-1] = strdup (str);
				// where give nestede value
				sprintf (str, "__end_%d_%d_%d", egg->lang.nfunctions, CTX - 1, egg->lang.nestedi[CTX - 1] - 1);
				emit->branch (egg, b, g, e, n, egg->lang.varsize, str);
				if (CTX > 0) {
					/* XXX .. */
				}
				rcc_reset_callname (egg);
				R_FREE (egg->lang.conditionstr);
			}	// else eprintf ("Unknown statement (%s)(%s)\n", cn, elem);
		}	// handle '{ ..'
	}
}