blkid_partition blkid_partlist_get_partition(blkid_partlist ls, int n)
{
	if (n < 0 || n >= ls->nparts)
		return NULL;

	return &ls->parts[n];
}