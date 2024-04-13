static struct mempolicy *shm_get_policy(struct vm_area_struct *vma,
					unsigned long addr)
{
	struct file *file = vma->vm_file;
	struct shm_file_data *sfd = shm_file_data(file);
	struct mempolicy *pol = NULL;

	if (sfd->vm_ops->get_policy)
		pol = sfd->vm_ops->get_policy(vma, addr);
	else if (vma->vm_policy)
		pol = vma->vm_policy;

	return pol;
}