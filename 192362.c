static inline ktime_t net_timedelta(ktime_t t)
{
	return ktime_sub(ktime_get_real(), t);
}