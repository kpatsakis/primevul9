static int shm_set_policy(struct vm_area_struct *vma, struct mempolicy *new)
{
	struct file *file = vma->vm_file;
	struct shm_file_data *sfd = shm_file_data(file);
	int err = 0;

	if (sfd->vm_ops->set_policy)
		err = sfd->vm_ops->set_policy(vma, new);
	return err;
}