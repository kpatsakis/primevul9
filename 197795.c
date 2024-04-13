int blkid_probe_invert_partitions_filter(blkid_probe pr)
{
	return __blkid_probe_invert_filter(pr, BLKID_CHAIN_PARTS);
}