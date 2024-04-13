R_API void r_egg_lang_free(REgg *egg) {
	int i, len;

	for (i = 0; i < egg->lang.nsyscalls; i++) {
 		R_FREE (egg->lang.syscalls[i].name);
 		R_FREE (egg->lang.syscalls[i].arg);
	}
	len = sizeof (egg->lang.ctxpush) / sizeof (char *);
	for (i = 0; i < len; i++) {
		R_FREE (egg->lang.ctxpush[i]);
	}
}