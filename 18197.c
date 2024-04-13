RZ_API bool rz_bin_dwarf_line_op_run(const RzBinDwarfLineHeader *hdr, RzBinDwarfSMRegisters *regs, RzBinDwarfLineOp *op,
	RZ_NULLABLE RzBinSourceLineInfoBuilder *bob, RZ_NULLABLE RzBinDwarfDebugInfo *info, RZ_NULLABLE RzBinDwarfLineFileCache fnc) {
	rz_return_val_if_fail(hdr && regs && op, false);
	switch (op->type) {
	case RZ_BIN_DWARF_LINE_OP_TYPE_STD:
		switch (op->opcode) {
		case DW_LNS_copy:
			if (bob) {
				store_line_sample(bob, hdr, regs, info, fnc);
			}
			regs->basic_block = DWARF_FALSE;
			break;
		case DW_LNS_advance_pc:
			regs->address += op->args.advance_pc * hdr->min_inst_len;
			break;
		case DW_LNS_advance_line:
			regs->line += op->args.advance_line;
			break;
		case DW_LNS_set_file:
			regs->file = op->args.set_file;
			break;
		case DW_LNS_set_column:
			regs->column = op->args.set_column;
			break;
		case DW_LNS_negate_stmt:
			regs->is_stmt = regs->is_stmt ? DWARF_FALSE : DWARF_TRUE;
			break;
		case DW_LNS_set_basic_block:
			regs->basic_block = DWARF_TRUE;
			break;
		case DW_LNS_const_add_pc:
			regs->address += rz_bin_dwarf_line_header_get_spec_op_advance_pc(hdr, 255);
			break;
		case DW_LNS_fixed_advance_pc:
			regs->address += op->args.fixed_advance_pc;
			break;
		case DW_LNS_set_prologue_end:
			regs->prologue_end = ~0;
			break;
		case DW_LNS_set_epilogue_begin:
			regs->epilogue_begin = ~0;
			break;
		case DW_LNS_set_isa:
			regs->isa = op->args.set_isa;
			break;
		default:
			return false;
		}
		break;
	case RZ_BIN_DWARF_LINE_OP_TYPE_EXT:
		switch (op->opcode) {
		case DW_LNE_end_sequence:
			regs->end_sequence = DWARF_TRUE;
			if (bob) {
				// closing entry
				rz_bin_source_line_info_builder_push_sample(bob, regs->address, 0, 0, NULL);
			}
			rz_bin_dwarf_line_header_reset_regs(hdr, regs);
			break;
		case DW_LNE_set_address:
			regs->address = op->args.set_address;
			break;
		case DW_LNE_define_file:
			break;
		case DW_LNE_set_discriminator:
			regs->discriminator = op->args.set_discriminator;
			break;
		default:
			return false;
		}
		break;
	case RZ_BIN_DWARF_LINE_OP_TYPE_SPEC:
		regs->address += rz_bin_dwarf_line_header_get_spec_op_advance_pc(hdr, op->opcode);
		regs->line += rz_bin_dwarf_line_header_get_spec_op_advance_line(hdr, op->opcode);
		if (bob) {
			store_line_sample(bob, hdr, regs, info, fnc);
		}
		regs->basic_block = DWARF_FALSE;
		regs->prologue_end = DWARF_FALSE;
		regs->epilogue_begin = DWARF_FALSE;
		regs->discriminator = 0;
		break;
	default:
		return false;
	}
	return true;
}