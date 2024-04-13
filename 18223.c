RZ_API ut64 rz_bin_dwarf_line_header_get_adj_opcode(const RzBinDwarfLineHeader *header, ut8 opcode) {
	rz_return_val_if_fail(header, 0);
	return opcode - header->opcode_base;
}