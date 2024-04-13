intel_pmu_save_and_restart_reload(struct perf_event *event, int count)
{
	struct hw_perf_event *hwc = &event->hw;
	int shift = 64 - x86_pmu.cntval_bits;
	u64 period = hwc->sample_period;
	u64 prev_raw_count, new_raw_count;
	s64 new, old;

	WARN_ON(!period);

	/*
	 * drain_pebs() only happens when the PMU is disabled.
	 */
	WARN_ON(this_cpu_read(cpu_hw_events.enabled));

	prev_raw_count = local64_read(&hwc->prev_count);
	rdpmcl(hwc->event_base_rdpmc, new_raw_count);
	local64_set(&hwc->prev_count, new_raw_count);

	/*
	 * Since the counter increments a negative counter value and
	 * overflows on the sign switch, giving the interval:
	 *
	 *   [-period, 0]
	 *
	 * the difference between two consequtive reads is:
	 *
	 *   A) value2 - value1;
	 *      when no overflows have happened in between,
	 *
	 *   B) (0 - value1) + (value2 - (-period));
	 *      when one overflow happened in between,
	 *
	 *   C) (0 - value1) + (n - 1) * (period) + (value2 - (-period));
	 *      when @n overflows happened in between.
	 *
	 * Here A) is the obvious difference, B) is the extension to the
	 * discrete interval, where the first term is to the top of the
	 * interval and the second term is from the bottom of the next
	 * interval and C) the extension to multiple intervals, where the
	 * middle term is the whole intervals covered.
	 *
	 * An equivalent of C, by reduction, is:
	 *
	 *   value2 - value1 + n * period
	 */
	new = ((s64)(new_raw_count << shift) >> shift);
	old = ((s64)(prev_raw_count << shift) >> shift);
	local64_add(new - old + count * period, &event->count);

	local64_set(&hwc->period_left, -new);

	perf_event_update_userpage(event);

	return 0;
}