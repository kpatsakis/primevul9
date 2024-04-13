static int myregwrite(RAnalEsil *esil, const char *name, ut64 *val) {
	AeaStats *stats = esil->user;
	if (oldregread && !strcmp (name, oldregread)) {
		r_list_pop (stats->regread);
		R_FREE (oldregread)
	}
	if (!IS_DIGIT (*name)) {
		if (!contains (stats->regs, name)) {
			r_list_push (stats->regs, strdup (name));
		}
		if (!contains (stats->regwrite, name)) {
			r_list_push (stats->regwrite, strdup (name));
		}
		char *v = r_str_newf ("%"PFMT64d, *val);
		if (!contains (stats->regvalues, v)) {
			r_list_push (stats->regvalues, strdup (v));
		}
		free (v);
	}
	return 0;
}