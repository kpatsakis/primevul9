static inline unsigned last_fd(struct fdtable *fdt)
{
	return fdt->max_fds - 1;
}