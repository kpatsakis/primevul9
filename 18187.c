static void store_line_sample(RzBinSourceLineInfoBuilder *bob, const RzBinDwarfLineHeader *hdr, RzBinDwarfSMRegisters *regs,
	RZ_NULLABLE RzBinDwarfDebugInfo *info, RZ_NULLABLE RzBinDwarfLineFileCache fnc) {
	const char *file = NULL;
	if (regs->file) {
		file = get_full_file_path(info, hdr, fnc, regs->file - 1);
	}
	rz_bin_source_line_info_builder_push_sample(bob, regs->address, (ut32)regs->line, (ut32)regs->column, file);
}