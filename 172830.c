static void rcc_internal_mathop(REgg *egg, const char *ptr, char *ep, char op) {
	char *p, *q, *oldp;	// avoid mem leak
	char type = ' ';
	char buf[64];	// may cause stack overflow
	oldp = p = q = strdup (ptr);
	if (get_op (&q)) {
		*q = '\x00';
	}
	REggEmit *e = egg->remit;
	while (*p && is_space (*p)) {
		p++;
	}
	if (is_var (p)) {
		p = r_egg_mkvar (egg, buf, p, 0);
		if (egg->lang.varxs == '*') {
			e->load (egg, p, egg->lang.varsize);
			R_FREE (oldp);
			oldp = p = strdup (e->regs (egg, 0));
			// XXX: which will go wrong in arm
			// for reg used in emit.load in arm is r7 not r0
		} else if (egg->lang.varxs == '&') {
			e->load_ptr (egg, p);
			R_FREE (oldp);
			oldp = p = strdup (e->regs (egg, 0));
		}
		type = ' ';
	} else {
		type = '$';
	}
	if (*p) {
		e->mathop (egg, op, egg->lang.varsize, type, p, ep);
	}
	if (p != oldp) {
		R_FREE (p);
	}
	R_FREE (oldp);
	R_FREE (ep);
}