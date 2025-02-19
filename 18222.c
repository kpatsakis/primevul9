static int init_die(RzBinDwarfDie *die, ut64 abbr_code, ut64 attr_count) {
	if (!die) {
		return -1;
	}
	if (attr_count) {
		die->attr_values = calloc(sizeof(RzBinDwarfAttrValue), attr_count);
		if (!die->attr_values) {
			return -1;
		}
	} else {
		die->attr_values = NULL;
	}
	die->abbrev_code = abbr_code;
	die->capacity = attr_count;
	die->count = 0;
	return 0;
}