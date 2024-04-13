cdf_getdays(int year)
{
	int days = 0;
	int y;

	for (y = CDF_BASE_YEAR; y < year; y++)
		days += isleap(y) + 365;
		
	return days;
}