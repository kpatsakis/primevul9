int blkid_partition_is_logical(blkid_partition par)
{
	return partition_get_logical_type(par) == 'L' ? TRUE : FALSE;
}