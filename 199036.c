void exit_shm(struct task_struct *task)
{
	struct ipc_namespace *ns = task->nsproxy->ipc_ns;
	struct shmid_kernel *shp, *n;

	if (list_empty(&task->sysvshm.shm_clist))
		return;

	/*
	 * If kernel.shm_rmid_forced is not set then only keep track of
	 * which shmids are orphaned, so that a later set of the sysctl
	 * can clean them up.
	 */
	if (!ns->shm_rmid_forced) {
		down_read(&shm_ids(ns).rwsem);
		list_for_each_entry(shp, &task->sysvshm.shm_clist, shm_clist)
			shp->shm_creator = NULL;
		/*
		 * Only under read lock but we are only called on current
		 * so no entry on the list will be shared.
		 */
		list_del(&task->sysvshm.shm_clist);
		up_read(&shm_ids(ns).rwsem);
		return;
	}

	/*
	 * Destroy all already created segments, that were not yet mapped,
	 * and mark any mapped as orphan to cover the sysctl toggling.
	 * Destroy is skipped if shm_may_destroy() returns false.
	 */
	down_write(&shm_ids(ns).rwsem);
	list_for_each_entry_safe(shp, n, &task->sysvshm.shm_clist, shm_clist) {
		shp->shm_creator = NULL;

		if (shm_may_destroy(ns, shp)) {
			shm_lock_by_ptr(shp);
			shm_destroy(ns, shp);
		}
	}

	/* Remove the list head from any segments still attached. */
	list_del(&task->sysvshm.shm_clist);
	up_write(&shm_ids(ns).rwsem);
}