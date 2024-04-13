static __always_inline u32 __flow_hash_words(u32 *words, u32 length, u32 keyval)
{
	return jhash2(words, length, keyval);
}