static inline bool __is_front_mergeable(struct extent_info *cur,
						struct extent_info *front)
{
	return __is_extent_mergeable(cur, front);
}