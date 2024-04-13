static void line_unit_free(RzBinDwarfLineUnit *unit) {
	if (!unit) {
		return;
	}
	line_header_fini(&unit->header);
	if (unit->ops) {
		for (size_t i = 0; i < unit->ops_count; i++) {
			rz_bin_dwarf_line_op_fini(&unit->ops[i]);
		}
		free(unit->ops);
	}
	free(unit);
}