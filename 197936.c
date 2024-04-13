read_rotation(const char *arg)
{
	int i;

	if (strcmp(arg, "auto"))
	{
		return -1;
	}

	i = fz_atoi(arg);

	i = i % 360;
	if (i % 90 != 0)
	{
		fprintf(stderr, "Ignoring invalid rotation\n");
		i = 0;
	}

	return i;
}