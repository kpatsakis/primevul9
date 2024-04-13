const char *blkid_partition_get_uuid(blkid_partition par)
{
	return *par->uuid ? par->uuid : NULL;
}