static inline u32 htab_map_hash(const void *key, u32 key_len, u32 hashrnd)
{
	return jhash(key, key_len, hashrnd);
}