static int cc_print(void *p, const char *k, const char *v) {
	if (!strcmp (v, "cc")) {
		r_cons_println (k);
	}
	return 1;
}