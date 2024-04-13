static int aio_ring_mmap(struct file *file, struct vm_area_struct *vma)
{
	vma->vm_ops = &generic_file_vm_ops;
	return 0;
}