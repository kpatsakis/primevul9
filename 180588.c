static inline int agaw_to_width(int agaw)
{
	return min_t(int, 30 + agaw * LEVEL_STRIDE, MAX_AGAW_WIDTH);
}