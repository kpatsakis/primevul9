is_valid_filename (const char *p)
{
	if (!*p)
		return 0;
	return strpbrk (p, "\\//:")? 0: 1;
}