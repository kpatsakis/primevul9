void peak_usb_get_ts_time(struct peak_time_ref *time_ref, u32 ts, ktime_t *time)
{
	/* protect from getting time before setting now */
	if (ktime_to_ns(time_ref->tv_host)) {
		u64 delta_us;

		delta_us = ts - time_ref->ts_dev_2;
		if (ts < time_ref->ts_dev_2)
			delta_us &= (1 << time_ref->adapter->ts_used_bits) - 1;

		delta_us += time_ref->ts_total;

		delta_us *= time_ref->adapter->us_per_ts_scale;
		delta_us >>= time_ref->adapter->us_per_ts_shift;

		*time = ktime_add_us(time_ref->tv_host_0, delta_us);
	} else {
		*time = ktime_get();
	}
}