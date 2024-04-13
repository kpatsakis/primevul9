static int shmctl_nolock(struct ipc_namespace *ns, int shmid,
			 int cmd, int version, void __user *buf)
{
	int err;
	struct shmid_kernel *shp;

	/* preliminary security checks for *_INFO */
	if (cmd == IPC_INFO || cmd == SHM_INFO) {
		err = security_shm_shmctl(NULL, cmd);
		if (err)
			return err;
	}

	switch (cmd) {
	case IPC_INFO:
	{
		struct shminfo64 shminfo;

		memset(&shminfo, 0, sizeof(shminfo));
		shminfo.shmmni = shminfo.shmseg = ns->shm_ctlmni;
		shminfo.shmmax = ns->shm_ctlmax;
		shminfo.shmall = ns->shm_ctlall;

		shminfo.shmmin = SHMMIN;
		if (copy_shminfo_to_user(buf, &shminfo, version))
			return -EFAULT;

		down_read(&shm_ids(ns).rwsem);
		err = ipc_get_maxid(&shm_ids(ns));
		up_read(&shm_ids(ns).rwsem);

		if (err < 0)
			err = 0;
		goto out;
	}
	case SHM_INFO:
	{
		struct shm_info shm_info;

		memset(&shm_info, 0, sizeof(shm_info));
		down_read(&shm_ids(ns).rwsem);
		shm_info.used_ids = shm_ids(ns).in_use;
		shm_get_stat(ns, &shm_info.shm_rss, &shm_info.shm_swp);
		shm_info.shm_tot = ns->shm_tot;
		shm_info.swap_attempts = 0;
		shm_info.swap_successes = 0;
		err = ipc_get_maxid(&shm_ids(ns));
		up_read(&shm_ids(ns).rwsem);
		if (copy_to_user(buf, &shm_info, sizeof(shm_info))) {
			err = -EFAULT;
			goto out;
		}

		err = err < 0 ? 0 : err;
		goto out;
	}
	case SHM_STAT:
	case IPC_STAT:
	{
		struct shmid64_ds tbuf;
		int result;

		rcu_read_lock();
		if (cmd == SHM_STAT) {
			shp = shm_obtain_object(ns, shmid);
			if (IS_ERR(shp)) {
				err = PTR_ERR(shp);
				goto out_unlock;
			}
			result = shp->shm_perm.id;
		} else {
			shp = shm_obtain_object_check(ns, shmid);
			if (IS_ERR(shp)) {
				err = PTR_ERR(shp);
				goto out_unlock;
			}
			result = 0;
		}

		err = -EACCES;
		if (ipcperms(ns, &shp->shm_perm, S_IRUGO))
			goto out_unlock;

		err = security_shm_shmctl(shp, cmd);
		if (err)
			goto out_unlock;

		memset(&tbuf, 0, sizeof(tbuf));
		kernel_to_ipc64_perm(&shp->shm_perm, &tbuf.shm_perm);
		tbuf.shm_segsz	= shp->shm_segsz;
		tbuf.shm_atime	= shp->shm_atim;
		tbuf.shm_dtime	= shp->shm_dtim;
		tbuf.shm_ctime	= shp->shm_ctim;
		tbuf.shm_cpid	= shp->shm_cprid;
		tbuf.shm_lpid	= shp->shm_lprid;
		tbuf.shm_nattch	= shp->shm_nattch;
		rcu_read_unlock();

		if (copy_shmid_to_user(buf, &tbuf, version))
			err = -EFAULT;
		else
			err = result;
		goto out;
	}
	default:
		return -EINVAL;
	}

out_unlock:
	rcu_read_unlock();
out:
	return err;
}