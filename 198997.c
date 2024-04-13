static int shm_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct shm_file_data *sfd = shm_file_data(file);
	int ret;

	/*
	 * In case of remap_file_pages() emulation, the file can represent
	 * removed IPC ID: propogate shm_lock() error to caller.
	 */
	ret = __shm_open(vma);
	if (ret)
		return ret;

	ret = sfd->file->f_op->mmap(sfd->file, vma);
	if (ret) {
		shm_close(vma);
		return ret;
	}
	sfd->vm_ops = vma->vm_ops;
#ifdef CONFIG_MMU
	WARN_ON(!sfd->vm_ops->fault);
#endif
	vma->vm_ops = &shm_vm_ops;
	return 0;
}