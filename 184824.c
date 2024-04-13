static u32 ip4_key_hashfn(const void *data, u32 len, u32 seed)
{
	return jhash2(data,
		      sizeof(struct frag_v4_compare_key) / sizeof(u32), seed);
}