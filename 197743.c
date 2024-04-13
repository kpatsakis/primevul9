blkid_partition blkid_partlist_get_partition_by_partno(blkid_partlist ls, int n)
{
	int i, nparts;
	blkid_partition par;

	nparts = blkid_partlist_numof_partitions(ls);
	for (i = 0; i < nparts; i++) {
		par = blkid_partlist_get_partition(ls, i);
		if (n == blkid_partition_get_partno(par))
			return par;
	}
	return NULL;
}