static inline void warp_clock(void)
{
	if (sys_tz.tz_minuteswest != 0) {
		struct timespec adjust;

		persistent_clock_is_local = 1;
		adjust.tv_sec = sys_tz.tz_minuteswest * 60;
		adjust.tv_nsec = 0;
		timekeeping_inject_offset(&adjust);
	}
}