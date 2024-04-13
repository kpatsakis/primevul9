static ut64 *next_append(ut64 *next, int *nexti, ut64 v) {
	ut64 *tmp_next = realloc (next, sizeof (ut64) * (1 + *nexti));
	if (!tmp_next) {
		return NULL;
	}
	next = tmp_next;
	next[*nexti] = v;
	(*nexti)++;
	return next;
}