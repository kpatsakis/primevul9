int blkid_partition_is_primary(blkid_partition par)
{
	return partition_get_logical_type(par) == 'P' ? TRUE : FALSE;
}