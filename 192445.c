static inline u32 __flow_hash_from_keys(struct flow_keys *keys, u32 keyval)
{
	u32 hash;

	__flow_hash_consistentify(keys);

	hash = __flow_hash_words((u32 *)flow_keys_hash_start(keys),
				 flow_keys_hash_length(keys), keyval);
	if (!hash)
		hash = 1;

	return hash;
}