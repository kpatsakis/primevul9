const char *blkid_partition_get_type_string(blkid_partition par)
{
	return *par->typestr ? par->typestr : NULL;
}