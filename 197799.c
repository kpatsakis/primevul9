static int blkid_partlist_set_parent(blkid_partlist ls, blkid_partition par)
{
	if (!ls)
		return -1;
	ls->next_parent = par;
	return 0;
}