int blkid_probe_set_partitions_flags(blkid_probe pr, int flags)
{
	pr->chains[BLKID_CHAIN_PARTS].flags = flags;
	return 0;
}