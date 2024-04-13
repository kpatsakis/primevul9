static const ut8 *parse_std_opcode(RzBinDwarfLineOp *op, const RzBinDwarfLineHeader *hdr, const ut8 *obuf, size_t len, ut8 opcode, bool big_endian) {
	rz_return_val_if_fail(op && hdr && obuf, NULL);
	const ut8 *buf = obuf;
	const ut8 *buf_end = obuf + len;

	op->type = RZ_BIN_DWARF_LINE_OP_TYPE_STD;
	op->opcode = opcode;
	switch (opcode) {
	case DW_LNS_advance_pc:
		buf = rz_uleb128(buf, buf_end - buf, &op->args.advance_pc, NULL);
		break;
	case DW_LNS_advance_line:
		buf = rz_leb128(buf, buf_end - buf, &op->args.advance_line);
		break;
	case DW_LNS_set_file:
		buf = rz_uleb128(buf, buf_end - buf, &op->args.set_file, NULL);
		break;
	case DW_LNS_set_column:
		buf = rz_uleb128(buf, buf_end - buf, &op->args.set_column, NULL);
		break;
	case DW_LNS_fixed_advance_pc:
		op->args.fixed_advance_pc = READ16(buf);
		break;
	case DW_LNS_set_isa:
		buf = rz_uleb128(buf, buf_end - buf, &op->args.set_isa, NULL);
		break;

	// known opcodes that take no args
	case DW_LNS_copy:
	case DW_LNS_negate_stmt:
	case DW_LNS_set_basic_block:
	case DW_LNS_const_add_pc:
	case DW_LNS_set_prologue_end:
	case DW_LNS_set_epilogue_begin:
		break;

	// unknown operands, skip the number of args given in the header.
	default: {
		size_t args_count = std_opcode_args_count(hdr, opcode);
		for (size_t i = 0; i < args_count; i++) {
			buf = rz_uleb128(buf, buf_end - buf, &op->args.advance_pc, NULL);
			if (!buf) {
				break;
			}
		}
	}
	}
	return buf;
}