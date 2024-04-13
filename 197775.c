blkid_loff_t blkid_partition_get_start(blkid_partition par)
{
	return (blkid_loff_t)par->start;
}