int blkid_is_nested_dimension(blkid_partition par,
			uint64_t start, uint64_t size)
{
	uint64_t pstart;
	uint64_t psize;

	if (!par)
		return 0;

	pstart = blkid_partition_get_start(par);
	psize = blkid_partition_get_size(par);

	if (start < pstart || start + size > pstart + psize)
		return 0;

	return 1;
}