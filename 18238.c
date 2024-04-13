static void free_die(RzBinDwarfDie *die) {
	size_t i;
	if (!die) {
		return;
	}
	for (i = 0; i < die->count; i++) {
		free_attr_value(&die->attr_values[i]);
	}
	RZ_FREE(die->attr_values);
}