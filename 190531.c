static inline bool __is_discard_front_mergeable(struct discard_info *cur,
			struct discard_info *front, unsigned int max_len)
{
	return __is_discard_mergeable(cur, front, max_len);
}