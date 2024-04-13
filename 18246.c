static HtUP *parse_loc_raw(HtUP /*<offset, List *<LocListEntry>*/ *loc_table, const ut8 *buf, size_t len, size_t addr_size,
	bool big_endian) {
	/* GNU has their own extensions GNU locviews that we can't parse */
	const ut8 *const buf_start = buf;
	const ut8 *buf_end = buf + len;
	/* for recognizing Base address entry */
	ut64 max_offset = get_max_offset(addr_size);

	ut64 address_base = 0; /* remember base of the loclist */
	ut64 list_offset = 0;

	RzBinDwarfLocList *loc_list = NULL;
	RzBinDwarfLocRange *range = NULL;
	while (buf && buf < buf_end) {
		ut64 start_addr = dwarf_read_address(addr_size, big_endian, &buf, buf_end);
		ut64 end_addr = dwarf_read_address(addr_size, big_endian, &buf, buf_end);

		if (start_addr == 0 && end_addr == 0) { /* end of list entry: 0, 0 */
			if (loc_list) {
				ht_up_insert(loc_table, loc_list->offset, loc_list);
				list_offset = buf - buf_start;
				loc_list = NULL;
			}
			address_base = 0;
			continue;
		} else if (start_addr == max_offset && end_addr != max_offset) {
			/* base address, DWARF2 doesn't have this type of entry, these entries shouldn't
			   be in the list, they are just informational entries for further parsing (address_base) */
			address_base = end_addr;
		} else { /* location list entry: */
			if (!loc_list) {
				loc_list = create_loc_list(list_offset);
			}
			/* TODO in future parse expressions to better structure in dwarf.c and not in dwarf_process.c */
			RzBinDwarfBlock *block = RZ_NEW0(RzBinDwarfBlock);
			block->length = READ16(buf);
			buf = fill_block_data(buf, buf_end, block);
			range = create_loc_range(start_addr + address_base, end_addr + address_base, block);
			if (!range) {
				free(block);
			}
			rz_list_append(loc_list->list, range);
			range = NULL;
		}
	}
	/* if for some reason end of list is missing, then loc_list would leak */
	if (loc_list) {
		free_loc_table_list(loc_list);
	}
	return loc_table;
}