int blkid_probe_reset_partitions_filter(blkid_probe pr)
{
	return __blkid_probe_reset_filter(pr, BLKID_CHAIN_PARTS);
}