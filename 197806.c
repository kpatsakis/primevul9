blkid_partition blkid_partlist_add_partition(blkid_partlist ls,
					blkid_parttable tab, uint64_t start, uint64_t size)
{
	blkid_partition par = new_partition(ls, tab);

	if (!par)
		return NULL;

	par->start = start;
	par->size = size;

	DBG(LOWPROBE, ul_debug("parts: add partition (%p start=%"
		PRIu64 ", size=%" PRIu64 ", table=%p)",
		par, par->start, par->size, tab));
	return par;
}