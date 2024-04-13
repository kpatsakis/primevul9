static inline bool __is_back_mergeable(struct extent_info *cur,
						struct extent_info *back)
{
	return __is_extent_mergeable(back, cur);
}