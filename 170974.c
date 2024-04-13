static inline int test_time_stamp(u64 delta)
{
	if (delta & TS_DELTA_TEST)
		return 1;
	return 0;
}