static void free_parttables(blkid_partlist ls)
{
	if (!ls || !ls->l_tabs.next)
		return;

	/* remove unassigned partition tables */
	while (!list_empty(&ls->l_tabs)) {
		blkid_parttable tab = list_entry(ls->l_tabs.next,
					struct blkid_struct_parttable, t_tabs);
		unref_parttable(tab);
	}
}