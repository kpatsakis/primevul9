adjust_partial_year_to_2020(int year)
{
	/*
	 * Adjust all dates toward 2020; this is effectively what happens when we
	 * assume '70' is 1970 and '69' is 2069.
	 */
	/* Force 0-69 into the 2000's */
	if (year < 70)
		return year + 2000;
	/* Force 70-99 into the 1900's */
	else if (year < 100)
		return year + 1900;
	/* Force 100-519 into the 2000's */
	else if (year < 520)
		return year + 2000;
	/* Force 520-999 into the 1000's */
	else if (year < 1000)
		return year + 1000;
	else
		return year;
}