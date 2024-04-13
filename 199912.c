static loff_t bad_file_llseek(struct file *file, loff_t offset, int origin)
{
	return -EIO;
}