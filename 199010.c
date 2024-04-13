static int __shm_open(struct vm_area_struct *vma)
{
	struct file *file = vma->vm_file;
	struct shm_file_data *sfd = shm_file_data(file);
	struct shmid_kernel *shp;

	shp = shm_lock(sfd->ns, sfd->id);

	if (IS_ERR(shp))
		return PTR_ERR(shp);

	shp->shm_atim = get_seconds();
	shp->shm_lprid = task_tgid_vnr(current);
	shp->shm_nattch++;
	shm_unlock(shp);
	return 0;
}