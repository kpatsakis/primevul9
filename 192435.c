static inline void *flow_keys_hash_start(struct flow_keys *flow)
{
	BUILD_BUG_ON(FLOW_KEYS_HASH_OFFSET % sizeof(u32));
	return (void *)flow + FLOW_KEYS_HASH_OFFSET;
}