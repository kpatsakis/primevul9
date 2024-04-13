int blkid_probe_enable_partitions(blkid_probe pr, int enable)
{
	pr->chains[BLKID_CHAIN_PARTS].enabled = enable;
	return 0;
}