void set_close_on_exec(unsigned int fd, int flag)
{
	struct files_struct *files = current->files;
	struct fdtable *fdt;
	spin_lock(&files->file_lock);
	fdt = files_fdtable(files);
	if (flag)
		__set_close_on_exec(fd, fdt);
	else
		__clear_close_on_exec(fd, fdt);
	spin_unlock(&files->file_lock);
}