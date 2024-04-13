static void set_nested(REgg *egg, const char *s) {
	int i = 0;
	if (CTX < 1) {
		return;
	}
	free (egg->lang.nested[CTX]);
	egg->lang.nested[CTX] = strdup (s);
	// egg->lang.nestedi[c]++;
	// seems not need to increase egg->lang.nestedi[c]
	/** clear inner levels **/
	for (i = 1; i < 10; i++) {
		// egg->lang.nestedi[context+i] = 0;
		R_FREE (egg->lang.nested[CTX + i]);
	}
}