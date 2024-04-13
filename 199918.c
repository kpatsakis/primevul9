static unsigned long bad_file_get_unmapped_area(struct file *file,
				unsigned long addr, unsigned long len,
				unsigned long pgoff, unsigned long flags)
{
	return -EIO;
}