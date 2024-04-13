long verbosity(void)
{
	if (!verbose)
		return 0;
	return *verbose;
}