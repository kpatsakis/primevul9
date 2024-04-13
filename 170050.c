int main(void)
{
	timelib_time time = timelib_strtotime("May 12");

	printf ("%04d-%02d-%02d %02d:%02d:%02d.%-5d %+04d %1d",
		time.y, time.m, time.d, time.h, time.i, time.s, time.f, time.z, time.dst);
	if (time.have_relative) {
		printf ("%3dY %3dM %3dD / %3dH %3dM %3dS",
			time.relative.y, time.relative.m, time.relative.d, time.relative.h, time.relative.i, time.relative.s);
	}
	if (time.have_weekday_relative) {
		printf (" / %d", time.relative.weekday);
	}
	if (time.have_weeknr_day) {
		printf(" / %dW%d", time.relative.weeknr_day.weeknr, time.relative.weeknr_day.dayofweek);
	}
	return 0;
}