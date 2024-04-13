blkid_loff_t blkid_partition_get_size(blkid_partition par)
{
	return (blkid_loff_t)par->size;
}