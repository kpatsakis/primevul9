blkid_partition blkid_partlist_get_parent(blkid_partlist ls)
{
	if (!ls)
		return NULL;
	return ls->next_parent;
}