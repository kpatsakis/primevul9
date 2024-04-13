static int expand_info(RzBinDwarfDebugInfo *info) {
	rz_return_val_if_fail(info && info->capacity == info->count, -1);

	RzBinDwarfCompUnit *tmp = realloc(info->comp_units,
		info->capacity * 2 * sizeof(RzBinDwarfCompUnit));
	if (!tmp) {
		return -1;
	}

	memset((ut8 *)tmp + info->capacity * sizeof(RzBinDwarfCompUnit),
		0, info->capacity * sizeof(RzBinDwarfCompUnit));

	info->comp_units = tmp;
	info->capacity *= 2;

	return 0;
}