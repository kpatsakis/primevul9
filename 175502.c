static inline struct hlist_nulls_head *select_bucket(struct bpf_htab *htab, u32 hash)
{
	return &__select_bucket(htab, hash)->head;
}