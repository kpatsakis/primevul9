struct timespec64 ns_to_timespec64(const s64 nsec)
{
	struct timespec64 ts;
	s32 rem;

	if (!nsec)
		return (struct timespec64) {0, 0};

	ts.tv_sec = div_s64_rem(nsec, NSEC_PER_SEC, &rem);
	if (unlikely(rem < 0)) {
		ts.tv_sec--;
		rem += NSEC_PER_SEC;
	}
	ts.tv_nsec = rem;

	return ts;
}