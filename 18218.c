RZ_API void rz_bin_dwarf_line_header_reset_regs(const RzBinDwarfLineHeader *hdr, RzBinDwarfSMRegisters *regs) {
	rz_return_if_fail(hdr && regs);
	regs->address = 0;
	regs->file = 1;
	regs->line = 1;
	regs->column = 0;
	regs->is_stmt = hdr->default_is_stmt;
	regs->basic_block = DWARF_FALSE;
	regs->end_sequence = DWARF_FALSE;
	regs->prologue_end = DWARF_FALSE;
	regs->epilogue_begin = DWARF_FALSE;
	regs->isa = 0;
}