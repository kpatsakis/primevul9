static int shm_fsync(struct file *file, loff_t start, loff_t end, int datasync)
{
	struct shm_file_data *sfd = shm_file_data(file);

	if (!sfd->file->f_op->fsync)
		return -EINVAL;
	return sfd->file->f_op->fsync(sfd->file, start, end, datasync);
}