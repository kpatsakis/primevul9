static int bad_file_mmap(struct file *file, struct vm_area_struct *vma)
{
	return -EIO;
}