static int shmctl_down(struct ipc_namespace *ns, int shmid, int cmd,
		       struct shmid_ds __user *buf, int version)
{
	struct kern_ipc_perm *ipcp;
	struct shmid64_ds shmid64;
	struct shmid_kernel *shp;
	int err;

	if (cmd == IPC_SET) {
		if (copy_shmid_from_user(&shmid64, buf, version))
			return -EFAULT;
	}

	down_write(&shm_ids(ns).rwsem);
	rcu_read_lock();

	ipcp = ipcctl_pre_down_nolock(ns, &shm_ids(ns), shmid, cmd,
				      &shmid64.shm_perm, 0);
	if (IS_ERR(ipcp)) {
		err = PTR_ERR(ipcp);
		goto out_unlock1;
	}

	shp = container_of(ipcp, struct shmid_kernel, shm_perm);

	err = security_shm_shmctl(shp, cmd);
	if (err)
		goto out_unlock1;

	switch (cmd) {
	case IPC_RMID:
		ipc_lock_object(&shp->shm_perm);
		/* do_shm_rmid unlocks the ipc object and rcu */
		do_shm_rmid(ns, ipcp);
		goto out_up;
	case IPC_SET:
		ipc_lock_object(&shp->shm_perm);
		err = ipc_update_perm(&shmid64.shm_perm, ipcp);
		if (err)
			goto out_unlock0;
		shp->shm_ctim = get_seconds();
		break;
	default:
		err = -EINVAL;
		goto out_unlock1;
	}

out_unlock0:
	ipc_unlock_object(&shp->shm_perm);
out_unlock1:
	rcu_read_unlock();
out_up:
	up_write(&shm_ids(ns).rwsem);
	return err;
}