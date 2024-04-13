static const ut8 *parse_line_header(
	RzBinFile *bf, const ut8 *buf, const ut8 *buf_end,
	RzBinDwarfLineHeader *hdr, ut64 offset_cur, bool big_endian) {
	rz_return_val_if_fail(hdr && bf && buf && buf_end, NULL);

	hdr->offset = offset_cur;
	hdr->is_64bit = false;
	hdr->unit_length = dwarf_read_initial_length(&hdr->is_64bit, big_endian, &buf, buf_end);
	hdr->version = READ16(buf);

	if (hdr->version == 5) {
		hdr->address_size = READ8(buf);
		hdr->segment_selector_size = READ8(buf);
	}

	hdr->header_length = dwarf_read_offset(hdr->is_64bit, big_endian, &buf, buf_end);

	const ut8 *tmp_buf = buf; // So I can skip parsing DWARF 5 headers for now

	if (buf_end - buf < 8) {
		return NULL;
	}
	hdr->min_inst_len = READ8(buf);
	if (hdr->version >= 4) {
		hdr->max_ops_per_inst = READ8(buf);
	}
	hdr->default_is_stmt = READ8(buf);
	hdr->line_base = (st8)READ8(buf);
	hdr->line_range = READ8(buf);
	hdr->opcode_base = READ8(buf);

	hdr->file_names = NULL;

	if (hdr->opcode_base > 1) {
		hdr->std_opcode_lengths = calloc(sizeof(ut8), hdr->opcode_base - 1);
		for (size_t i = 1; i < hdr->opcode_base; i++) {
			if (buf + 2 > buf_end) {
				hdr->opcode_base = i;
				break;
			}
			hdr->std_opcode_lengths[i - 1] = READ8(buf);
		}
	} else {
		hdr->std_opcode_lengths = NULL;
	}
	// TODO finish parsing of source files out of DWARF 5 header
	// for now we skip
	if (hdr->version == 5) {
		tmp_buf += hdr->header_length;
		return tmp_buf;
	}

	if (hdr->version <= 4) {
		buf = parse_line_header_source(bf, buf, buf_end, hdr);
	} else {
		buf = NULL;
	}

	return buf;
}