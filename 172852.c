static int parseinlinechar(REgg *egg, char c) {
	/* skip until '{' */
	if (c == '{') {	/* XXX: repeated code!! */
		rcc_context (egg, 1);
		egg->lang.inlinectr++;
		if (egg->lang.inlinectr == 1) {
			return 0;
		}
	} else if (egg->lang.inlinectr == 0) {
		return 0;
	}

	/* capture body */
	if (c == '}') {	/* XXX: repeated code!! */
		if (CTX < 2) {
			rcc_context (egg, -1);
			egg->lang.slurp = 0;
			egg->lang.mode = NORMAL;
			egg->lang.inlinectr = 0;
			if (!egg->lang.dstvar && egg->lang.dstval == egg->lang.syscallbody) {
				egg->lang.dstval = NULL;
				return 1;
			} else	/* register */
			if (egg->lang.dstval && egg->lang.dstvar) {
				egg->lang.dstval[egg->lang.ndstval] = '\0';
				// printf(" /* END OF INLINE (%s)(%s) */\n", egg->lang.dstvar, egg->lang.dstval);
				egg->lang.inlines[egg->lang.ninlines].name = strdup (skipspaces (egg->lang.dstvar));
				egg->lang.inlines[egg->lang.ninlines].body = strdup (skipspaces (egg->lang.dstval));
				egg->lang.ninlines++;
				R_FREE (egg->lang.dstvar);
				R_FREE (egg->lang.dstval);
				return 1;
			}
			eprintf ("Parse error\n");
		}
	}
	if (egg->lang.dstval) {
		egg->lang.dstval[egg->lang.ndstval++] = c;
		egg->lang.dstval[egg->lang.ndstval] = 0;
	}
	return 0;
}