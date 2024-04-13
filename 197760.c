blkid_parttable blkid_partlist_new_parttable(blkid_partlist ls,
				const char *type, uint64_t offset)
{
	blkid_parttable tab;

	tab = calloc(1, sizeof(struct blkid_struct_parttable));
	if (!tab)
		return NULL;
	tab->type = type;
	tab->offset = offset;
	tab->parent = ls->next_parent;

	INIT_LIST_HEAD(&tab->t_tabs);
	list_add_tail(&tab->t_tabs, &ls->l_tabs);

	DBG(LOWPROBE, ul_debug("parts: create a new partition table "
		       "(%p, type=%s, offset=%"PRId64")", tab, type, offset));
	return tab;
}