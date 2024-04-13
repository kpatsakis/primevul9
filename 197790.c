blkid_partition blkid_partlist_get_partition_by_start(blkid_partlist ls, uint64_t start)
{
	int i, nparts;
	blkid_partition par;

	nparts = blkid_partlist_numof_partitions(ls);
	for (i = 0; i < nparts; i++) {
		par = blkid_partlist_get_partition(ls, i);
		if ((uint64_t) blkid_partition_get_start(par) == start)
			return par;
	}
	return NULL;
}