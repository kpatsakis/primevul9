static void unref_parttable(blkid_parttable tab)
{
	tab->nparts--;

	if (tab->nparts <= 0) {
		list_del(&tab->t_tabs);
		free(tab);
	}
}