static unsigned int count_open_files(struct fdtable *fdt)
{
	unsigned int size = fdt->max_fds;
	unsigned int i;

	/* Find the last open fd */
	for (i = size / BITS_PER_LONG; i > 0; ) {
		if (fdt->open_fds[--i])
			break;
	}
	i = (i + 1) * BITS_PER_LONG;
	return i;
}