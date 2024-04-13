static const ut8 *info_comp_unit_read_hdr(const ut8 *buf, const ut8 *buf_end, RzBinDwarfCompUnitHdr *hdr, bool big_endian) {
	// 32-bit vs 64-bit dwarf formats
	// http://www.dwarfstd.org/doc/Dwarf3.pdf section 7.4
	hdr->length = READ32(buf);
	if (hdr->length == (ut32)DWARF_INIT_LEN_64) { // then its 64bit
		hdr->length = READ64(buf);
		hdr->is_64bit = true;
	}
	const ut8 *tmp = buf; // to calculate header size
	hdr->version = READ16(buf);
	if (hdr->version == 5) {
		hdr->unit_type = READ8(buf);
		hdr->address_size = READ8(buf);
		hdr->abbrev_offset = dwarf_read_offset(hdr->is_64bit, big_endian, &buf, buf_end);

		if (hdr->unit_type == DW_UT_skeleton || hdr->unit_type == DW_UT_split_compile) {
			hdr->dwo_id = READ8(buf);
		} else if (hdr->unit_type == DW_UT_type || hdr->unit_type == DW_UT_split_type) {
			hdr->type_sig = READ64(buf);
			hdr->type_offset = dwarf_read_offset(hdr->is_64bit, big_endian, &buf, buf_end);
		}
	} else {
		hdr->abbrev_offset = dwarf_read_offset(hdr->is_64bit, big_endian, &buf, buf_end);
		hdr->address_size = READ8(buf);
	}
	hdr->header_size = buf - tmp; // header size excluding length field
	return buf;
}