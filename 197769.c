int blkid_partition_set_type(blkid_partition par, int type)
{
	par->type = type;
	return 0;
}