static inline unsigned mph_lookup(
		unsigned code,
		const short *g_table, unsigned g_table_size,
		const unsigned *table, unsigned table_size)
{
	short g = g_table[mph_hash(0, code) % g_table_size];

	unsigned idx;
	if (g <= 0) {
		idx = -g;
	} else {
		idx = mph_hash(g, code) % table_size;
	}

	if (table[2*idx] == code) {
		return table[2*idx + 1];
	}
	return CODE_NOT_FOUND;
}