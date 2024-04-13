static phys_addr_t root_entry_lctp(struct root_entry *re)
{
	if (!(re->lo & 1))
		return 0;

	return re->lo & VTD_PAGE_MASK;
}