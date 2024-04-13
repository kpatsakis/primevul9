static int shm_release(struct inode *ino, struct file *file)
{
	struct shm_file_data *sfd = shm_file_data(file);

	put_ipc_ns(sfd->ns);
	shm_file_data(file) = NULL;
	kfree(sfd);
	return 0;
}