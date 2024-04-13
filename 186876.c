cdf_getmonth(int year, int days)
{
	size_t m;

	for (m = 0; m < sizeof(mdays) / sizeof(mdays[0]); m++) {
		days -= mdays[m];
		if (m == 1 && isleap(year))
			days--;
		if (days <= 0)
			return (int)m;
	}
	return (int)m;
}