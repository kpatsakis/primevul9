static int bad_file_flock(struct file *filp, int cmd, struct file_lock *fl)
{
	return -EIO;
}