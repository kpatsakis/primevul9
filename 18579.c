static inline void __clear_close_on_exec(unsigned int fd, struct fdtable *fdt)
{
	if (test_bit(fd, fdt->close_on_exec))
		__clear_bit(fd, fdt->close_on_exec);
}