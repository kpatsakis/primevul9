RZ_API void rz_bin_dwarf_loc_free(HtUP /*<offset, RzBinDwarfLocList*>*/ *loc_table) {
	rz_return_if_fail(loc_table);
	loc_table->opt.freefn = free_loc_table_entry;
	ht_up_free(loc_table);
}