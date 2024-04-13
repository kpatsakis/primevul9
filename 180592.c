static phys_addr_t root_entry_uctp(struct root_entry *re)
{
	if (!(re->hi & 1))
		return 0;

	return re->hi & VTD_PAGE_MASK;
}