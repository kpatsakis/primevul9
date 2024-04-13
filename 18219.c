static size_t parse_opcodes(const ut8 *obuf,
	size_t len, const RzBinDwarfLineHeader *hdr, RzVector *ops_out,
	RzBinDwarfSMRegisters *regs, RZ_NULLABLE RzBinSourceLineInfoBuilder *bob, RZ_NULLABLE RzBinDwarfDebugInfo *info,
	RZ_NULLABLE RzBinDwarfLineFileCache fnc, bool big_endian, ut8 target_addr_size) {
	const ut8 *buf, *buf_end;
	ut8 opcode;

	if (!obuf || !len) {
		return 0;
	}
	buf = obuf;
	buf_end = obuf + len;

	while (buf < buf_end) {
		opcode = *buf++;
		RzBinDwarfLineOp op = { 0 };
		if (!opcode) {
			buf = parse_ext_opcode(&op, hdr, buf, (buf_end - buf), big_endian, target_addr_size);
		} else if (opcode >= hdr->opcode_base) {
			// special opcode without args, no further parsing needed
			op.type = RZ_BIN_DWARF_LINE_OP_TYPE_SPEC;
			op.opcode = opcode;
		} else {
			buf = parse_std_opcode(&op, hdr, buf, (buf_end - buf), opcode, big_endian);
		}
		if (!buf) {
			break;
		}
		if (bob) {
			rz_bin_dwarf_line_op_run(hdr, regs, &op, bob, info, fnc);
		}
		if (ops_out) {
			rz_vector_push(ops_out, &op);
		} else {
			rz_bin_dwarf_line_op_fini(&op);
		}
	}
	if (!buf) {
		return 0;
	}
	return (size_t)(buf - obuf); // number of bytes we've moved by
}