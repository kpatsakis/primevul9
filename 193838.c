static inline u64 wrap_max(u64 x, u64 y)
{
	return (s64)(x - y) > 0 ? x : y;
}