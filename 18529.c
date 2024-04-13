get_strtab(Pe *pe)
{
	static void *ret = NULL;
	uint32_t *ptr;
	intptr_t intret = 0;
	struct pe_hdr pehdr;
	void *map = NULL;
	size_t map_size = 0;

	if (ret)
		return ret;

	if (pe_getpehdr(pe, &pehdr) == NULL)
		pereterr(NULL, "invalid PE file header");

	map = pe_rawfile(pe, &map_size);
	if (!map || map_size < 1)
		return 0;

	if (pehdr.symbol_table == 0)
		return NULL;

	intret = (intptr_t)pehdr.symbol_table;
	intret += pehdr.symbols * sizeof(struct pe_symtab_entry);

	ptr = (uint32_t *)((intptr_t)map + intret);
	if (!check_pointer_and_size(pe, ptr, 4))
		pereterr(NULL, "invalid string table start");

	if (!check_pointer_and_size(pe, ptr, *ptr))
		pereterr(NULL, "invalid string table size");
	ret = ptr;
	return ret;
}