static const ut8 *parse_ext_opcode(RzBinDwarfLineOp *op, const RzBinDwarfLineHeader *hdr, const ut8 *obuf, size_t len,
	bool big_endian, ut8 target_addr_size) {
	rz_return_val_if_fail(op && hdr && obuf, NULL);
	const ut8 *buf = obuf;
	const ut8 *buf_end = obuf + len;

	ut64 op_len;
	buf = rz_uleb128(buf, len, &op_len, NULL);
	// op_len must fit and be at least 1 (for the opcode byte)
	if (!buf || buf >= buf_end || !op_len || buf_end - buf < op_len) {
		return NULL;
	}

	ut8 opcode = *buf++;
	op->type = RZ_BIN_DWARF_LINE_OP_TYPE_EXT;
	op->opcode = opcode;

	switch (opcode) {
	case DW_LNE_set_address: {
		ut8 addr_size = hdr->address_size;
		if (hdr->version < 5) { // address_size in header only starting with Dwarf 5
			addr_size = target_addr_size;
		}
		op->args.set_address = dwarf_read_address(addr_size, big_endian, &buf, buf_end);
		break;
	}
	case DW_LNE_define_file: {
		size_t fn_len = rz_str_nlen((const char *)buf, buf_end - buf);
		char *fn = malloc(fn_len + 1);
		if (!fn) {
			return NULL;
		}
		memcpy(fn, buf, fn_len);
		fn[fn_len] = 0;
		op->args.define_file.filename = fn;
		buf += fn_len + 1;
		if (buf + 1 < buf_end) {
			buf = rz_uleb128(buf, buf_end - buf, &op->args.define_file.dir_index, NULL);
		}
		if (buf && buf + 1 < buf_end) {
			buf = rz_uleb128(buf, buf_end - buf, NULL, NULL);
		}
		if (buf && buf + 1 < buf_end) {
			buf = rz_uleb128(buf, buf_end - buf, NULL, NULL);
		}
		break;
	}
	case DW_LNE_set_discriminator:
		buf = rz_uleb128(buf, buf_end - buf, &op->args.set_discriminator, NULL);
		break;
	case DW_LNE_end_sequence:
	default:
		buf += op_len - 1;
		break;
	}
	return buf;
}