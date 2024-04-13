int blkid_partition_is_extended(blkid_partition par)
{
	return partition_get_logical_type(par) == 'E' ? TRUE : FALSE;
}