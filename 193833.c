static u64 sched_clock_remote(struct sched_clock_data *scd)
{
	struct sched_clock_data *my_scd = this_scd();
	u64 this_clock, remote_clock;
	u64 *ptr, old_val, val;

#if BITS_PER_LONG != 64
again:
	/*
	 * Careful here: The local and the remote clock values need to
	 * be read out atomic as we need to compare the values and
	 * then update either the local or the remote side. So the
	 * cmpxchg64 below only protects one readout.
	 *
	 * We must reread via sched_clock_local() in the retry case on
	 * 32bit as an NMI could use sched_clock_local() via the
	 * tracer and hit between the readout of
	 * the low32bit and the high 32bit portion.
	 */
	this_clock = sched_clock_local(my_scd);
	/*
	 * We must enforce atomic readout on 32bit, otherwise the
	 * update on the remote cpu can hit inbetween the readout of
	 * the low32bit and the high 32bit portion.
	 */
	remote_clock = cmpxchg64(&scd->clock, 0, 0);
#else
	/*
	 * On 64bit the read of [my]scd->clock is atomic versus the
	 * update, so we can avoid the above 32bit dance.
	 */
	sched_clock_local(my_scd);
again:
	this_clock = my_scd->clock;
	remote_clock = scd->clock;
#endif

	/*
	 * Use the opportunity that we have both locks
	 * taken to couple the two clocks: we take the
	 * larger time as the latest time for both
	 * runqueues. (this creates monotonic movement)
	 */
	if (likely((s64)(remote_clock - this_clock) < 0)) {
		ptr = &scd->clock;
		old_val = remote_clock;
		val = this_clock;
	} else {
		/*
		 * Should be rare, but possible:
		 */
		ptr = &my_scd->clock;
		old_val = this_clock;
		val = remote_clock;
	}

	if (cmpxchg64(ptr, old_val, val) != old_val)
		goto again;

	return val;
}