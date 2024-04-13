static int parsedatachar(REgg *egg, char c) {
	char *str;
	int i, j;

	if (!egg->lang.dstval) {
		return 0;
	}
	/* skip until '{' */
	if (c == '{') {	/* XXX: repeated code!! */
		rcc_context (egg, 1);
		if (++(egg->lang.inlinectr) == 1) {
			return egg->lang.ndstval = 0;
		}
	} else if (egg->lang.inlinectr == 0) {
		/* capture value between parenthesis foo@data(NNN) { ... } */
		if (c == ')') {
			egg->lang.stackframe = atoi (egg->lang.dstval);
			egg->lang.ndstval = 0;
		} else {
			egg->lang.dstval[egg->lang.ndstval++] = c;
		}
		return 0;
	}
	/* capture body */
	if (c == '}') {	/* XXX: repeated code!! */
		if (CTX < 2) {
			egg->lang.inlinectr = 0;
			rcc_context (egg, -1);
			egg->lang.slurp = 0;
			egg->lang.mode = NORMAL;
			/* register */
			if (egg->lang.dstval && egg->lang.dstvar) {
				egg->lang.dstval[egg->lang.ndstval] = '\0';
				egg->remit->comment (egg, "data (%s)(%s)size=(%d)\n",
					egg->lang.dstvar, egg->lang.dstval, egg->lang.stackframe);
				r_egg_printf (egg, ".data\n");
				for (str = egg->lang.dstval; is_space (*str); str++) {
					;
				}
				j = (egg->lang.stackframe)? egg->lang.stackframe: 1;
				/* emit label */
				r_egg_printf (egg, "%s:\n", egg->lang.dstvar);
				for (i = 1; i <= j; i++) {
					if (*str == '"') {
						r_egg_printf (egg, ".ascii %s%s\n", egg->lang.dstval, (i == j)? "\"\\x00\"": "");
					} else {
						r_egg_printf (egg, ".long %s\n", egg->lang.dstval);
					}
				}
				r_egg_printf (egg, ".text\n");
				R_FREE (egg->lang.dstvar);
				R_FREE (egg->lang.dstval);
				egg->lang.ndstval = 0;
				CTX = 0;
				return 1;
			}
		}
	}
	if (egg->lang.dstval) {
		egg->lang.dstval[egg->lang.ndstval++] = c;
	}
	return 0;
}