ext4_xattr_create_cache(void)
{
	return mb_cache_create(HASH_BUCKET_BITS);
}