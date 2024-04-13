static inline int width_to_agaw(int width)
{
	return DIV_ROUND_UP(width - 30, LEVEL_STRIDE);
}