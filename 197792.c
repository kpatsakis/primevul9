static void reset_partlist(blkid_partlist ls)
{
	if (!ls)
		return;

	free_parttables(ls);

	if (ls->next_partno) {
		/* already initialized - reset */
		int tmp_nparts = ls->nparts_max;
		blkid_partition tmp_parts = ls->parts;

		memset(ls, 0, sizeof(struct blkid_struct_partlist));

		ls->nparts_max = tmp_nparts;
		ls->parts = tmp_parts;
	}

	ls->nparts = 0;
	ls->next_partno = 1;
	INIT_LIST_HEAD(&ls->l_tabs);

	DBG(LOWPROBE, ul_debug("partlist reset"));
}