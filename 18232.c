RZ_API ut64 rz_bin_dwarf_line_header_get_spec_op_advance_pc(const RzBinDwarfLineHeader *header, ut8 opcode) {
	rz_return_val_if_fail(header, 0);
	if (!header->line_range) {
		// to dodge division by zero
		return 0;
	}
	ut8 adj_opcode = rz_bin_dwarf_line_header_get_adj_opcode(header, opcode);
	return (adj_opcode / header->line_range) * header->min_inst_len;
}