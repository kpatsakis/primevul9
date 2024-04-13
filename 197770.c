static int partition_get_logical_type(blkid_partition par)
{
	blkid_parttable tab;

	if (!par)
		return -1;

	tab = blkid_partition_get_table(par);
	if (!tab || !tab->type)
		return -1;

	if (tab->parent)
		return 'L';  /* report nested partitions as logical */

	if (!strcmp(tab->type, "dos")) {
		if (par->partno > 4)
			return 'L';	/* logical */

	        if(par->type == MBR_DOS_EXTENDED_PARTITION ||
                   par->type == MBR_W95_EXTENDED_PARTITION ||
		   par->type == MBR_LINUX_EXTENDED_PARTITION)
			return 'E';
	}
	return 'P';
}