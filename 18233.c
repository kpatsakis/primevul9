static RzBinDwarfLineInfo *parse_line_raw(RzBinFile *binfile, const ut8 *obuf,
	ut64 len, RzBinDwarfLineInfoMask mask, bool big_endian, RZ_NULLABLE RzBinDwarfDebugInfo *info) {
	// Dwarf 3 Standard 6.2 Line Number Information
	rz_return_val_if_fail(binfile && obuf, NULL);

	const ut8 *buf = obuf;
	const ut8 *buf_start = buf;
	const ut8 *buf_end = obuf + len;
	const ut8 *tmpbuf = NULL;
	ut64 buf_size;

	// Dwarf < 5 needs this size to be supplied from outside
	RzBinObject *o = binfile->o;
	ut8 target_addr_size = o && o->info && o->info->bits ? o->info->bits / 8 : 4;

	RzBinDwarfLineInfo *li = RZ_NEW0(RzBinDwarfLineInfo);
	if (!li) {
		return NULL;
	}
	li->units = rz_list_newf((RzListFree)line_unit_free);
	if (!li->units) {
		free(li);
		return NULL;
	}

	RzBinSourceLineInfoBuilder bob;
	if (mask & RZ_BIN_DWARF_LINE_INFO_MASK_LINES) {
		rz_bin_source_line_info_builder_init(&bob);
	}

	// each iteration we read one header AKA comp. unit
	while (buf <= buf_end) {
		RzBinDwarfLineUnit *unit = RZ_NEW0(RzBinDwarfLineUnit);
		if (!unit) {
			break;
		}

		// How much did we read from the compilation unit
		size_t bytes_read = 0;
		// calculate how much we've read by parsing header
		// because header unit_length includes itself
		buf_size = buf_end - buf;

		tmpbuf = buf;
		buf = parse_line_header(binfile, buf, buf_end, &unit->header, buf - buf_start, big_endian);
		if (!buf) {
			line_unit_free(unit);
			break;
		}

		bytes_read = buf - tmpbuf;

		RzBinDwarfSMRegisters regs;
		rz_bin_dwarf_line_header_reset_regs(&unit->header, &regs);

		// If there is more bytes in the buffer than size of the header
		// It means that there has to be another header/comp.unit
		buf_size = RZ_MIN(buf_size, unit->header.unit_length + (unit->header.is_64bit * 8 + 4)); // length field + rest of the unit
		if (buf_size <= bytes_read) {
			// no info or truncated
			line_unit_free(unit);
			continue;
		}
		if (buf_size > (buf_end - buf) + bytes_read || buf > buf_end) {
			line_unit_free(unit);
			break;
		}
		size_t tmp_read = 0;

		RzVector ops;
		if (mask & RZ_BIN_DWARF_LINE_INFO_MASK_OPS) {
			rz_vector_init(&ops, sizeof(RzBinDwarfLineOp), NULL, NULL);
		}

		RzBinDwarfLineFileCache fnc = NULL;
		if (mask & RZ_BIN_DWARF_LINE_INFO_MASK_LINES) {
			fnc = rz_bin_dwarf_line_header_new_file_cache(&unit->header);
		}

		// we read the whole compilation unit (that might be composed of more sequences)
		do {
			// reads one whole sequence
			tmp_read = parse_opcodes(buf, buf_size - bytes_read, &unit->header,
				(mask & RZ_BIN_DWARF_LINE_INFO_MASK_OPS) ? &ops : NULL, &regs,
				(mask & RZ_BIN_DWARF_LINE_INFO_MASK_LINES) ? &bob : NULL,
				info, fnc, big_endian, target_addr_size);
			bytes_read += tmp_read;
			buf += tmp_read; // Move in the buffer forward
		} while (bytes_read < buf_size && tmp_read != 0); // if nothing is read -> error, exit

		rz_bin_dwarf_line_header_free_file_cache(&unit->header, fnc);

		if (mask & RZ_BIN_DWARF_LINE_INFO_MASK_OPS) {
			unit->ops_count = rz_vector_len(&ops);
			unit->ops = rz_vector_flush(&ops);
			rz_vector_fini(&ops);
		}

		if (!tmp_read) {
			line_unit_free(unit);
			break;
		}
		rz_list_push(li->units, unit);
	}
	if (mask & RZ_BIN_DWARF_LINE_INFO_MASK_LINES) {
		li->lines = rz_bin_source_line_info_builder_build_and_fini(&bob);
	}
	return li;
}