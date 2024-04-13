static unsigned int sane_fdtable_size(struct fdtable *fdt, unsigned int max_fds)
{
	unsigned int count;

	count = count_open_files(fdt);
	if (max_fds < NR_OPEN_DEFAULT)
		max_fds = NR_OPEN_DEFAULT;
	return min(count, max_fds);
}