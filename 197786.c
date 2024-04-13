int blkid_partition_set_utf8name(blkid_partition par, const unsigned char *name,
		size_t len, int enc)
{
	if (!par)
		return -1;

	blkid_encode_to_utf8(enc, par->name, sizeof(par->name), name, len);
	blkid_rtrim_whitespace(par->name);
	return 0;
}