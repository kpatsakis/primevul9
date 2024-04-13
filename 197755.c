int blkid_probe_filter_partitions_type(blkid_probe pr, int flag, char *names[])
{
	return __blkid_probe_filter_types(pr, BLKID_CHAIN_PARTS, flag, names);
}