blkid_parttable blkid_partlist_get_table(blkid_partlist ls)
{
	if (list_empty(&ls->l_tabs))
		return NULL;

	return list_entry(ls->l_tabs.next,
			struct blkid_struct_parttable, t_tabs);
}