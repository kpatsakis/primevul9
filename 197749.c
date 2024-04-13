int blkid_partition_set_type_string(blkid_partition par,
		const unsigned char *type, size_t len)
{
	set_string((unsigned char *) par->typestr,
			sizeof(par->typestr), type, len);
	return 0;
}