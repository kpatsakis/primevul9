void peak_usb_update_ts_now(struct peak_time_ref *time_ref, u32 ts_now)
{
	time_ref->ts_dev_2 = ts_now;

	/* should wait at least two passes before computing */
	if (ktime_to_ns(time_ref->tv_host) > 0) {
		u32 delta_ts = time_ref->ts_dev_2 - time_ref->ts_dev_1;

		if (time_ref->ts_dev_2 < time_ref->ts_dev_1)
			delta_ts &= (1 << time_ref->adapter->ts_used_bits) - 1;

		time_ref->ts_total += delta_ts;
	}
}