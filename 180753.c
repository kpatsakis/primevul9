static __init int setup_numa_zonelist_order(char *s)
{
	if (!s)
		return 0;

	return __parse_numa_zonelist_order(s);
}