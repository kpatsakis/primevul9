static int bad_file_lock(struct file *file, int cmd, struct file_lock *fl)
{
	return -EIO;
}