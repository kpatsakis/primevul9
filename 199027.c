static void shm_close(struct vm_area_struct *vma)
{
	struct file *file = vma->vm_file;
	struct shm_file_data *sfd = shm_file_data(file);
	struct shmid_kernel *shp;
	struct ipc_namespace *ns = sfd->ns;

	down_write(&shm_ids(ns).rwsem);
	/* remove from the list of attaches of the shm segment */
	shp = shm_lock(ns, sfd->id);

	/*
	 * We raced in the idr lookup or with shm_destroy().
	 * Either way, the ID is busted.
	 */
	if (WARN_ON_ONCE(IS_ERR(shp)))
		goto done; /* no-op */

	shp->shm_lprid = task_tgid_vnr(current);
	shp->shm_dtim = get_seconds();
	shp->shm_nattch--;
	if (shm_may_destroy(ns, shp))
		shm_destroy(ns, shp);
	else
		shm_unlock(shp);
done:
	up_write(&shm_ids(ns).rwsem);
}