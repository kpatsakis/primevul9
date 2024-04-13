struct file *filp_open(const char *filename, int flags, umode_t mode)
{
	struct filename name = {.name = filename};
	return file_open_name(&name, flags, mode);
}