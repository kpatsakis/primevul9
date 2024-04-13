static inline bool __is_discard_back_mergeable(struct discard_info *cur,
			struct discard_info *back, unsigned int max_len)
{
	return __is_discard_mergeable(back, cur, max_len);
}