int blkid_partition_set_flags(blkid_partition par, unsigned long long flags)
{
	par->flags = flags;
	return 0;
}