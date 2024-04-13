static long shm_fallocate(struct file *file, int mode, loff_t offset,
			  loff_t len)
{
	struct shm_file_data *sfd = shm_file_data(file);

	if (!sfd->file->f_op->fallocate)
		return -EOPNOTSUPP;
	return sfd->file->f_op->fallocate(file, mode, offset, len);
}