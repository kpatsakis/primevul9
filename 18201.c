RZ_API void rz_bin_dwarf_line_op_fini(RzBinDwarfLineOp *op) {
	rz_return_if_fail(op);
	if (op->type == RZ_BIN_DWARF_LINE_OP_TYPE_EXT && op->opcode == DW_LNE_define_file) {
		free(op->args.define_file.filename);
	}
}