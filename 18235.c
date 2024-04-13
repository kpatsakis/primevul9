static const ut8 *parse_attr_value(const ut8 *obuf, int obuf_len,
	RzBinDwarfAttrDef *def, RzBinDwarfAttrValue *value,
	const RzBinDwarfCompUnitHdr *hdr,
	const ut8 *debug_str, size_t debug_str_len,
	bool big_endian) {

	const ut8 *buf = obuf;
	const ut8 *buf_end = obuf + obuf_len;
	size_t j;

	rz_return_val_if_fail(def && value && hdr && obuf && obuf_len >= 1, NULL);

	value->attr_form = def->attr_form;
	value->attr_name = def->attr_name;
	value->block.data = NULL;
	value->string.content = NULL;
	value->string.offset = 0;

	// http://www.dwarfstd.org/doc/DWARF4.pdf#page=161&zoom=100,0,560
	switch (def->attr_form) {
	case DW_FORM_addr:
		value->kind = DW_AT_KIND_ADDRESS;
		switch (hdr->address_size) {
		case 1:
			value->address = READ8(buf);
			break;
		case 2:
			value->address = READ16(buf);
			break;
		case 4:
			value->address = READ32(buf);
			break;
		case 8:
			value->address = READ64(buf);
			break;
		default:
			eprintf("DWARF: Unexpected pointer size: %u\n", (unsigned)hdr->address_size);
			return NULL;
		}
		break;
	case DW_FORM_data1:
		value->kind = DW_AT_KIND_CONSTANT;
		value->uconstant = READ8(buf);
		break;
	case DW_FORM_data2:
		value->kind = DW_AT_KIND_CONSTANT;
		value->uconstant = READ16(buf);
		break;
	case DW_FORM_data4:
		value->kind = DW_AT_KIND_CONSTANT;
		value->uconstant = READ32(buf);
		break;
	case DW_FORM_data8:
		value->kind = DW_AT_KIND_CONSTANT;
		value->uconstant = READ64(buf);
		break;
	case DW_FORM_data16: // TODO Fix this, right now I just read the data, but I need to make storage for it
		value->kind = DW_AT_KIND_CONSTANT;
		value->uconstant = READ64(buf);
		value->uconstant = READ64(buf);
		break;
	case DW_FORM_sdata:
		value->kind = DW_AT_KIND_CONSTANT;
		buf = rz_leb128(buf, buf_end - buf, &value->sconstant);
		break;
	case DW_FORM_udata:
		value->kind = DW_AT_KIND_CONSTANT;
		buf = rz_uleb128(buf, buf_end - buf, &value->uconstant, NULL);
		break;
	case DW_FORM_string:
		value->kind = DW_AT_KIND_STRING;
		value->string.content = *buf ? strdup((const char *)buf) : NULL;
		buf += (strlen((const char *)buf) + 1);
		break;
	case DW_FORM_block1:
		value->kind = DW_AT_KIND_BLOCK;
		value->block.length = READ8(buf);
		buf = fill_block_data(buf, buf_end, &value->block);
		break;
	case DW_FORM_block2:
		value->kind = DW_AT_KIND_BLOCK;
		value->block.length = READ16(buf);
		if (value->block.length > 0) {
			value->block.data = calloc(sizeof(ut8), value->block.length);
			if (!value->block.data) {
				return NULL;
			}
			for (j = 0; j < value->block.length; j++) {
				value->block.data[j] = READ8(buf);
			}
		}
		break;
	case DW_FORM_block4:
		value->kind = DW_AT_KIND_BLOCK;
		value->block.length = READ32(buf);
		buf = fill_block_data(buf, buf_end, &value->block);
		break;
	case DW_FORM_block: // variable length ULEB128
		value->kind = DW_AT_KIND_BLOCK;
		buf = rz_uleb128(buf, buf_end - buf, &value->block.length, NULL);
		if (!buf || buf >= buf_end) {
			return NULL;
		}
		buf = fill_block_data(buf, buf_end, &value->block);
		break;
	case DW_FORM_flag:
		value->kind = DW_AT_KIND_FLAG;
		value->flag = READ8(buf);
		break;
	// offset in .debug_str
	case DW_FORM_strp:
		value->kind = DW_AT_KIND_STRING;
		value->string.offset = dwarf_read_offset(hdr->is_64bit, big_endian, &buf, buf_end);
		if (debug_str && value->string.offset < debug_str_len) {
			value->string.content =
				strdup((const char *)(debug_str + value->string.offset));
		} else {
			value->string.content = NULL; // Means malformed DWARF, should we print error message?
		}
		break;
	// offset in .debug_info
	case DW_FORM_ref_addr:
		value->kind = DW_AT_KIND_REFERENCE;
		value->reference = dwarf_read_offset(hdr->is_64bit, big_endian, &buf, buf_end);
		break;
	// This type of reference is an offset from the first byte of the compilation
	// header for the compilation unit containing the reference
	case DW_FORM_ref1:
		value->kind = DW_AT_KIND_REFERENCE;
		value->reference = hdr->unit_offset + READ8(buf);
		break;
	case DW_FORM_ref2:
		value->kind = DW_AT_KIND_REFERENCE;
		value->reference = hdr->unit_offset + READ16(buf);
		break;
	case DW_FORM_ref4:
		value->kind = DW_AT_KIND_REFERENCE;
		value->reference = hdr->unit_offset + READ32(buf);
		break;
	case DW_FORM_ref8:
		value->kind = DW_AT_KIND_REFERENCE;
		value->reference = hdr->unit_offset + READ64(buf);
		break;
	case DW_FORM_ref_udata:
		value->kind = DW_AT_KIND_REFERENCE;
		// uleb128 is enough to fit into ut64?
		buf = rz_uleb128(buf, buf_end - buf, &value->reference, NULL);
		value->reference += hdr->unit_offset;
		break;
	// offset in a section other than .debug_info or .debug_str
	case DW_FORM_sec_offset:
		value->kind = DW_AT_KIND_REFERENCE;
		value->reference = dwarf_read_offset(hdr->is_64bit, big_endian, &buf, buf_end);
		break;
	case DW_FORM_exprloc:
		value->kind = DW_AT_KIND_BLOCK;
		buf = rz_uleb128(buf, buf_end - buf, &value->block.length, NULL);
		if (!buf || buf >= buf_end) {
			return NULL;
		}
		buf = fill_block_data(buf, buf_end, &value->block);
		break;
	// this means that the flag is present, nothing is read
	case DW_FORM_flag_present:
		value->kind = DW_AT_KIND_FLAG;
		value->flag = true;
		break;
	case DW_FORM_ref_sig8:
		value->kind = DW_AT_KIND_REFERENCE;
		value->reference = READ64(buf);
		break;
	// offset into .debug_line_str section, can't parse the section now, so we just skip
	case DW_FORM_strx:
		value->kind = DW_AT_KIND_STRING;
		// value->string.offset = dwarf_read_offset (hdr->is_64bit, big_endian, &buf, buf_end);
		// if (debug_str && value->string.offset < debug_line_str_len) {
		// 	value->string.content =
		// 		strdup ((const char *)(debug_str + value->string.offset));
		// } else {
		// 	value->string.content = NULL; // Means malformed DWARF, should we print error message?
		// }
		break;
	case DW_FORM_strx1:
		value->kind = DW_AT_KIND_STRING;
		value->string.offset = READ8(buf);
		break;
	case DW_FORM_strx2:
		value->kind = DW_AT_KIND_STRING;
		value->string.offset = READ16(buf);
		break;
	case DW_FORM_strx3: // TODO Add 3 byte int read
		value->kind = DW_AT_KIND_STRING;
		buf += 3;
		break;
	case DW_FORM_strx4:
		value->kind = DW_AT_KIND_STRING;
		value->string.offset = READ32(buf);
		break;
	case DW_FORM_implicit_const:
		value->kind = DW_AT_KIND_CONSTANT;
		value->uconstant = def->special;
		break;
	/*  addrx* forms : The index is relative to the value of the
		DW_AT_addr_base attribute of the associated compilation unit.
	    index into an array of addresses in the .debug_addr section.*/
	case DW_FORM_addrx:
		value->kind = DW_AT_KIND_ADDRESS;
		buf = rz_uleb128(buf, buf_end - buf, &value->address, NULL);
		break;
	case DW_FORM_addrx1:
		value->kind = DW_AT_KIND_ADDRESS;
		value->address = READ8(buf);
		break;
	case DW_FORM_addrx2:
		value->kind = DW_AT_KIND_ADDRESS;
		value->address = READ16(buf);
		break;
	case DW_FORM_addrx3:
		// I need to add 3byte endianess free read here TODO
		value->kind = DW_AT_KIND_ADDRESS;
		buf += 3;
		break;
	case DW_FORM_addrx4:
		value->kind = DW_AT_KIND_ADDRESS;
		value->address = READ32(buf);
		break;
	case DW_FORM_line_ptr: // offset in a section .debug_line_str
	case DW_FORM_strp_sup: // offset in a section .debug_line_str
		value->kind = DW_AT_KIND_STRING;
		value->string.offset = dwarf_read_offset(hdr->is_64bit, big_endian, &buf, buf_end);
		// if (debug_str && value->string.offset < debug_line_str_len) {
		// 	value->string.content =
		// 		strdupsts
		break;
	// offset in the supplementary object file
	case DW_FORM_ref_sup4:
		value->kind = DW_AT_KIND_REFERENCE;
		value->reference = READ32(buf);
		break;
	case DW_FORM_ref_sup8:
		value->kind = DW_AT_KIND_REFERENCE;
		value->reference = READ64(buf);
		break;
	// An index into the .debug_loc
	case DW_FORM_loclistx:
		value->kind = DW_AT_KIND_LOCLISTPTR;
		value->reference = dwarf_read_offset(hdr->is_64bit, big_endian, &buf, buf_end);
		break;
		// An index into the .debug_rnglists
	case DW_FORM_rnglistx:
		value->kind = DW_AT_KIND_ADDRESS;
		buf = rz_uleb128(buf, buf_end - buf, &value->address, NULL);
		break;
	default:
		eprintf("Unknown DW_FORM 0x%02" PFMT64x "\n", def->attr_form);
		value->uconstant = 0;
		return NULL;
	}
	return buf;
}