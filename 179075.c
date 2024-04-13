void peak_usb_set_ts_now(struct peak_time_ref *time_ref, u32 ts_now)
{
	if (ktime_to_ns(time_ref->tv_host_0) == 0) {
		/* use monotonic clock to correctly compute further deltas */
		time_ref->tv_host_0 = ktime_get();
		time_ref->tv_host = ktime_set(0, 0);
	} else {
		/*
		 * delta_us should not be >= 2^32 => delta should be < 4294s
		 * handle 32-bits wrapping here: if count of s. reaches 4200,
		 * reset counters and change time base
		 */
		if (ktime_to_ns(time_ref->tv_host)) {
			ktime_t delta = ktime_sub(time_ref->tv_host,
						  time_ref->tv_host_0);
			if (ktime_to_ns(delta) > (4200ull * NSEC_PER_SEC)) {
				time_ref->tv_host_0 = time_ref->tv_host;
				time_ref->ts_total = 0;
			}
		}

		time_ref->tv_host = ktime_get();
		time_ref->tick_count++;
	}

	time_ref->ts_dev_1 = time_ref->ts_dev_2;
	peak_usb_update_ts_now(time_ref, ts_now);
}