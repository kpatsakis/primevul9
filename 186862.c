cdf_getday(int year, int days)
{
	size_t m;

	for (m = 0; m < sizeof(mdays) / sizeof(mdays[0]); m++) {
		int sub = mdays[m] + (m == 1 && isleap(year));
		if (days < sub)
			return days;
		days -= sub;
	}
	return days;
}