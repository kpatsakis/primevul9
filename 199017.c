static unsigned long shm_get_unmapped_area(struct file *file,
	unsigned long addr, unsigned long len, unsigned long pgoff,
	unsigned long flags)
{
	struct shm_file_data *sfd = shm_file_data(file);

	return sfd->file->f_op->get_unmapped_area(sfd->file, addr, len,
						pgoff, flags);
}