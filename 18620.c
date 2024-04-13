static inline void __set_close_on_exec(unsigned int fd, struct fdtable *fdt)
{
	__set_bit(fd, fdt->close_on_exec);
}