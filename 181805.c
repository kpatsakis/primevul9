static int myregread(RAnalEsil *esil, const char *name, ut64 *val, int *len) {
	AeaStats *stats = esil->user;
	if (!IS_DIGIT (*name)) {
		if (!contains (stats->inputregs, name)) {
			if (!contains (stats->regwrite, name)) {
				r_list_push (stats->inputregs, strdup (name));
			}
		}
		if (!contains (stats->regs, name)) {
			r_list_push (stats->regs, strdup (name));
		}
		if (!contains (stats->regread, name)) {
			r_list_push (stats->regread, strdup (name));
		}
	}
	return 0;
}