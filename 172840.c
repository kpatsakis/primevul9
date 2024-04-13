static void rcc_pusharg(REgg *egg, char *str) {
	REggEmit *e = egg->remit;
	char buf[64], *p = r_egg_mkvar (egg, buf, str, 0);
	if (!p) {
		return;
	}
	R_FREE (egg->lang.ctxpush[CTX]);
	egg->lang.ctxpush[CTX] = strdup (p);	// INDEX IT WITH NARGS OR CONTEXT?!?
	egg->lang.nargs++;
	if (egg->lang.pushargs) {
		e->push_arg (egg, egg->lang.varxs, egg->lang.nargs, p);
	}
	// egg->lang.ctxpush[context+egg->lang.nbrackets] = strdup(str); // use egg->lang.nargs??? (in callname)
	free (p);
}